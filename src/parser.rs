use std::iter::Peekable;
use std::ascii::AsciiExt;

use tokenizer;

// Disclaimer: I'm not entirely sure why I need to borrow the iterator
// as mutable everywhere. Don't judge me. I'll figure out.

#[derive(Debug)]
pub enum Datatype {
    Integer,
    Short,
    String,
    Float
}

#[derive(Debug)]
pub enum Node {
    Empty,
    Root { functions: Vec<Node> },
    Function { datatype: Datatype, parameters: Vec<Node> }
}

type PeekableTokenIterator = Peekable<tokenizer::TokenIterator>;
pub type WrappedNode = Option<Result<Node, String>>;

fn function(it: &mut PeekableTokenIterator) -> WrappedNode {
    let peeked = it.peek();

    let token = match peeked {
        // If it's a Text, eat it
        Some(val @ &tokenizer::Token::Text(_)) => val,

        Some(ref what) => {
            return Some(Err(format!("Expected Token::Text, got {:?}", what)));
        },

        // Absolutely nothing found
        None => return None
    };

    // token should be a Token::Text here
    
    let text = match token {
        &tokenizer::Token::Text(ref val) => val,
        
        // We only have text tokens
        _ => unreachable!()
    };

    if !(&text[..]).eq_ignore_ascii_case(b"Function") {
        return Some(Err(format!(
            "Expected Text({:?}), got {:?}", b"function", token)));

    };

    None
}

fn root(it: &mut PeekableTokenIterator) -> WrappedNode {
    let mut functions = Vec::<Node>::new();

    loop {
        let val = function(it);

        println!("{:?}", val);
        match val {
            Some(Ok(function)) => functions.push(function),

            // Error - pass error along
            Some(Err(_)) => return val,

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
