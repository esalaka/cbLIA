use std::iter::Peekable;
use std::ascii::AsciiExt;

use tokenizer;

// Disclaimer: I'm not entirely sure why I need to borrow the iterator
// as mutable everywhere. Don't judge me. I'll figure out.

macro_rules! error_expected {
    ($expected:expr, $got:expr) => {{
        return Some(Err(format!("{}:{}: Expected {}, got {:?}",
                                file!(), line!(), $expected, $got)));
    }}
}

#[derive(Debug)]
pub enum Datatype {
    Unknown,
    String,
    Float,
    Integer
}

#[derive(Debug)]
pub enum Node {
    Empty,
    Root { functions: Vec<Node> },
    Function { name: Vec<u8>, datatype: Datatype, arguments: Vec<Node> },
    Argument { name: Vec<u8>, datatype: Datatype }
}

type PeekableTokenIterator = Peekable<tokenizer::TokenIterator>;
pub type WrappedNode = Option<Result<Node, String>>;
pub type WrappedNodeVec = Option<Result<Vec<Node>, String>>;

fn datatype_sigil(it: &mut PeekableTokenIterator) -> Datatype {
    // Never errors, only mutates state if valid
    match it.peek() {
        Some(&Ok(tokenizer::Token::Dollar)) => {
            // Consume
            it.next();
            Datatype::String
        },

        Some(&Ok(tokenizer::Token::Hash)) => {
            it.next();
            Datatype::Float
        },

        // Any other case there IS no sigil
        Some(&Ok(_)) => {
            Datatype::Integer
        }

        // Something weird happened
        _ => Datatype::Unknown
    }
}

fn single_argument(it: &mut PeekableTokenIterator) -> WrappedNode {
    // We want one Text, 0-1 sigils
    // If peek gives RParen, we're fine
    // See further below for what the true/false do
    if match it.peek() {
        // No more arguments
        Some(&Ok(tokenizer::Token::RParen)) => {
            // Consume and return
            true
        }

        // An argument!
        Some(&Ok(tokenizer::Token::Text(_))) => {
            // Do not consume, do not return
            false
        },

        Some(&Ok(ref what)) => error_expected!("Text", what),

        none @ None => error_expected!("Text", none),

        Some(&Err(ref msg)) => error_expected!("Text", msg)
    } {
        it.next();
        return None;
    }

    // We have a Text at this point
    let name = match it.next() {
        Some(Ok(tokenizer::Token::Text(n))) => n,
    
        Some(Err(msg)) => error_expected!("Text", msg),

        _ => unreachable!()
    };

    // I really like this lil' function
    let datatype = datatype_sigil(it);

    // Now check if there's defaults
    match &it.peek() {
        &Some(&Ok(tokenizer::Token::Equals)) => {
            // Consume that
            {
                it.next();
            }
            
            // Now we match the NEXT peek
            if match it.peek() {
                Some(&Ok(tokenizer::Token::String(_))) => {
                    // Consume that, too
                    {
                        true
                    }
                },

                Some(&Ok(tokenizer::Token::Number(_))) => {
                    true
                },

                Some(ref what) => error_expected!("Number or String", what),
                none @ None => error_expected!("Number or String", none)
            } {
                it.next();
            }
        },

        // Don't mind if there's none
        _ => {}
    }
    // Now we should've dealt with potential defaults.

    // That's it!
    // Check for a comma or RParen, and return
    // This might look perverse but basically, the match is the
    // condition of the if statement, and the it.next() part is executed
    // if it returns true. We do it this way because it is still
    // borrowed to match before it ends.
    if match it.peek() {
        Some(&Ok(tokenizer::Token::RParen)) => {
            // Don't consume the RParen, yet
            false
        },

        Some(&Ok(tokenizer::Token::Comma)) => {
            // Consume comma though
            true
        },


        Some(ref what) => error_expected!("Comma", what),

        // We're clever and assume an EOF before ) means you forgot the
        // ) and not a lot of commas and params
        none @ None => error_expected!("RParen", none)
    } {
        it.next();
    }

    Some(Ok(Node::Argument { name: name, datatype: datatype }))
}

fn function_arguments(it: &mut PeekableTokenIterator) -> WrappedNodeVec {
    // Find the LParen
    {
        match it.peek() {
            // On LParen, carry on
            Some(&Ok(tokenizer::Token::LParen)) => {},

            Some(ref what) => error_expected!("LParen", what),

            // Absolutely nothing found
            none @ None => error_expected!("LParen", none)
        }

        // Consume LParen
        it.next();
    }

    // What we'll return
    let mut values = Vec::<Node>::new();
    loop {
        // This deals with RParen too because... Yeah, why not
        match single_argument(it) {
            Some(Ok(argument)) => values.push(argument),

            // Errors propagated
            // The intermediate type is, unfortunately, different
            Some(Err(e @ _)) => return Some(Err(e)),

            // RParen
            None => break
        }
    }

    // Return
    Some(Ok(values))
}

fn function(it: &mut PeekableTokenIterator) -> WrappedNode {
    // So that we can reborrow it later
    let text = {
        // Check we're getting text
        // This'll return with error if it's not a Text
        match it.peek() {
            // If it's a Text, carry on
            Some(&Ok(tokenizer::Token::Text(_))) => {},

            // If not...
            Some(ref what) => error_expected!("Text", what),

            // Absolutely nothing found
            None => {
                // Only in this case we can return None
                // A line began with no token at all => EOF
                return None;
            }
        }

        // Now we know it's a token::Text
        match it.next() {
            Some(Ok(tokenizer::Token::Text(val @ _))) => val,

            // So the code must not be broken
            _ => unreachable!()
        }
    };

    // Slice all of text, check that it's equal to "Function"
    if !(&text[..]).eq_ignore_ascii_case(b"Function") {
        error_expected!(format!("Text({:?})", b"function"),
                        format!("Text({:?})", text));
    };

    // Keyword Function was found, carry on
    // Now we'll match for the function name
    let function_name = {
        match it.peek() {
            Some(&Ok(tokenizer::Token::Text(_))) => {},

            Some(ref what) => error_expected!("Text", what),

            // This time None is an error too
            none @ None => error_expected!("Text", none)
        }

        // Return the actual value for function_name
        match it.next() {
            Some(Ok(tokenizer::Token::Text(val @ _))) => val,
            
            _ => unreachable!()
        }
    };

    // Check for datatype
    // Never errors, so we can just do it like this
    let datatype = datatype_sigil(it);

    // Find arguments
    // This returns a vec!
    let arguments = match function_arguments(it) {
        Some(Ok(args)) => args,

        // Propagate errors
        // We have to do type conversion here
        Some(Err(e @ _)) => return Some(Err(e)),

        // None means just LParen RParen
        None => Vec::<Node>::new()
    };

    // This actually matches a line so grab the EOL too
    match it.peek() {
        // What we want
        Some(&Ok(tokenizer::Token::EOL)) => {},

        Some(ref what) => error_expected!("EOL", what),

        // Interestingly, this is fine too
        // (It means end of file)
        None => {}
    }

    // Consume the next token!
    // We can consume None, too, I believe.
    it.next();

    // And return!
    Some(Ok(Node::Function {
        name: function_name,
        datatype: datatype,
        arguments: arguments
    }))
}

fn root(it: &mut PeekableTokenIterator) -> WrappedNode {
    let mut functions = Vec::<Node>::new();

    loop {
        match function(it) {
            Some(Ok(function)) => functions.push(function),

            // Error - pass error along
            val @ Some(Err(_)) => return val,

            // No more functions, no error
            None => break
        }
    }

    // Return node, or None
    if functions.is_empty() {
        None
    }
    else {
        Some(Ok(Node::Root { functions: functions }))
    }
}

pub fn parse(it: tokenizer::TokenIterator) -> Result<Node, String> {
    let mut peekable = it.peekable();

    let result = root(&mut peekable);

    match result {
        // Got a valid node, passing it along
        Some(node @ Ok(_)) => node,

        // Error! Pass that along too
        Some(err @ Err(_)) => err,

        // We got nothing
        None => Ok(Node::Empty)
    }
}
