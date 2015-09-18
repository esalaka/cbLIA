use std::io::Bytes;
use std::io::Read;
use std::iter::Peekable;
use std::fs::File;

macro_rules! emit_token_number {
    ($buf:ident) => {{
        // The following involves some magic
        // First convert the buffer - a string - into a slice
        return Some(
            Token::Number(String::from_utf8_lossy(&$buf[..])
                          .parse().unwrap()));

    }}
}
macro_rules! emit_token_text {
    ($buf:ident) => {{
        return Some(Token::Text($buf.clone()));
    }}
}

macro_rules! emit_token_string {
    ($buf:ident) => {{
        return Some(Token::String($buf.clone()));
    }}
}

#[derive(Debug)]
enum Mode {
    None,
    Newline,
    Text,
    Number,
    Decimal,
    String
}

#[derive(Debug)]
pub enum Token {
    Text(Vec<u8>),
    String(Vec<u8>),
    Number(f64),
    LParen,
    RParen,
    Dollar,
    Hash,
    Equals,
    Comma,
    EOL
}

pub struct TokenIterator {
    iterator: Peekable<Bytes<File>>
}

impl TokenIterator {
    pub fn new(file: File) -> TokenIterator {
        TokenIterator{ iterator: file.bytes().peekable() }
    }
}

impl Iterator for TokenIterator {
    type Item = Token;

    // The actual tokenisation happens here
    fn next(&mut self) -> Option<Self::Item> {
        let mut mode = Mode::None;
        let mut buf = Vec::<u8>::new();

        loop {
            let next = self.iterator.next();
            let peek = self.iterator.peek();

            println!("{:?} {:?} {:?}", next, peek, mode);

            // Returning None stops iteration
            let byte = match next {
                None => return None,
                Some(Err(_)) => return None,
                Some(Ok(value)) => value
            };

            let peeked = match peek {
                None => None,
                Some(&Err(_)) => None,
                Some(&Ok(ref value)) => Some(value)
            };


            match mode {
                Mode::None => match byte {
                    b'\r' => {
                        mode = Mode::Newline;
                    },
                    
                    // Letters
                    // These can't be a macro, either? :c
                    c @ b'_' |
                    c @ b'A' ... b'Z' |
                    c @ b'a' ... b'z' |
                    c @ 0xC0 ... 0xD6 |
                    c @ 0xD8 ... 0xF6 |
                    c @ 0xF8 ... 0xFF => {
                        // Clear token buffer
                        buf.clear();
                        // Insert current char
                        buf.push(c);
                        // Go to text mode if next also letter
                        match peeked {
                            Some(&peek_byte) => match peek_byte {
                                b'_' |
                                b'0' ... b'9' |
                                b'A' ... b'Z' |
                                b'a' ... b'z' |
                                0xC0 ... 0xD6 |
                                0xD8 ... 0xF6 |
                                0xF8 ... 0xFF => {
                                    mode = Mode::Text;
                                },
                                _ => {
                                    // Emit text token
                                    emit_token_text!(buf);
                                }
                            },
                            
                            None => {
                                // Also emit text token
                                emit_token_text!(buf);
                            }
                        }
                    },

                    // String
                    b'"' => {
                        // Clear token buffer
                        buf.clear();
                        // Don't insert current char - it's irrelevant, it's always "
                        // We don't even match peeked - we just go to Mode::String
                        mode = Mode::String;
                    },

                    // Number
                    c @ b'0' ... b'9' => {
                        buf.clear();
                        buf.push(c);

                        match peeked {
                            Some(&peek_byte) => match peek_byte {
                                b'0' ... b'9' => {
                                    // Goto number mode
                                    mode = Mode::Number;
                                }

                                b'.' => {
                                    // Goto decimal mode
                                    mode = Mode::Decimal;
                                },

                                _ => {
                                    // Emit number token
                                    emit_token_number!(buf);
                                }
                            },
                            None => {
                                // Also emit number token
                                emit_token_number!(buf);
                            }
                        }
                    },

                    // Negative number
                    c @ b'-' => {
                        buf.clear();
                        buf.push(c);

                        match peeked {
                            Some(&peek_byte) => match peek_byte {
                                b'0' ... b'9' => {
                                    // Goto number mode
                                    mode = Mode::Number;
                                }
                                _ => {
                                    // Invalid!
                                    panic!("Minus encountered without number");
                                }
                            },
                            None => {
                                // Invalid!
                                panic!("Minus encountered without number");
                            }
                        }
                    },

                    // Opening paren
                    b'(' => {
                        // Just emit it
                        return Some(Token::LParen);
                    },

                    // Closing paren
                    b')' => {
                        // Just emit it
                        return Some(Token::RParen);
                    },

                    b'$' => {
                        return Some(Token::Dollar);
                    },

                    b'#' => {
                        return Some(Token::Hash);
                    },

                    b'=' => {
                        return Some(Token::Equals);
                    },

                    b',' => {
                        return Some(Token::Comma);
                    },

                    // Skip spaces
                    b' ' => {},

                    b'\n' => {
                        panic!("UNIX newline encountered");
                    }

                    b @ _ => {
                        panic!("Invalid or unhandled byte {:?} encountered",
                               b);
                    }

                },

                Mode::Newline => match byte {
                    b'\n' => {
                        // Emit EOL, we're good
                        return Some(Token::EOL);
                    }
                    _ => panic!("CR without corresponding LF in input file")
                },

                // We've already peeked at byte if we're here
                // That means we know we have something!
                // Push *current* (not peeked!) byte
                Mode::Text => {
                    buf.push(byte);
                    match peeked {
                        Some(&peek_byte) => match peek_byte {
                                // NOTE: THIS IS NOT THE SAME PATTERN AS
                                // THE PREVIOUS LETTER PATTERN!!
                                b'_' |
                                b'0' ... b'9' |
                                b'A' ... b'Z' |
                                b'a' ... b'z' |
                                0xC0 ... 0xD6 |
                                0xD8 ... 0xF6 |
                                0xF8 ... 0xFF => {
                                    // Carry on
                                },

                                // Valid varname ends
                                _ => {
                                    // Emit text token
                                    emit_token_text!(buf);
                                }
                        },

                        None => {
                            // Well, let's go back to normal mode?
                            // Also emit text token
                            emit_token_text!(buf);
                        }
                    }
                },

                Mode::Number => {
                    // Push current byte
                    buf.push(byte);
                    match peeked {
                        Some(&peek_byte) => match peek_byte {
                                b'0' ... b'9' => {
                                    // Carry on...
                                },

                                // Decimal point found
                                b'.' => {
                                    // Change modes
                                    mode = Mode::Decimal;
                                },

                                // Valid number ends
                                _ => {
                                    // Emit number token
                                    emit_token_number!(buf);
                                }
                        },

                        None => {
                            // Also emit number token
                            emit_token_number!(buf);
                        }
                    }
                },

                Mode::Decimal => {
                    buf.push(byte);
                    match peeked {
                        Some(&peek_byte) => match peek_byte {
                            b'0' ... b'9' => {
                                // Yes, good
                            },

                            b'.' => {
                                panic!("Only one decimal point allowed in a number!");
                            },

                            _ => {
                                // Emit number token
                                emit_token_number!(buf);
                            }
                        },
                        
                        None => {
                            // Also emit number token
                            emit_token_number!(buf);
                        }
                    }
                },

                Mode::String => {
                    // The current byte might be the closing "
                    match byte {
                        b'"' => {
                            emit_token_string!(buf);
                        },

                        c @ _ => {
                            // Nope! Push it.
                            buf.push(c);
                        }
                    }

                    // Kinda strange how there's no peeky logic here
                    // Guess it's what happens when ~~delimiters~~
                }
            }
                            
        }
    }
}
