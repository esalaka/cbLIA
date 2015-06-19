use std::io::Bytes;
use std::io::Read;
use std::iter::Peekable;
use std::fs::File;

macro_rules! emit_token_number {
    ($buf:ident) => {{
        println!("Token::Number emitted");
        // The following involves some magic
        return Some(
            Token::Number((*String::from_utf8_lossy(&*$buf.into_boxed_slice()))
                          .parse::<i32>().unwrap()));

    }}
}
macro_rules! emit_token_text {
    ($buf:ident) => {{
        println!("Token::Text emitted");
        return Some(Token::Text($buf.clone()));
    }}
}

#[derive(Debug)]
enum Mode {
    None,
    Newline,
    Text,
    Number
}

pub enum Token {
    Text(Vec<u8>),
    Number(i32),
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
                        println!("Token::LParen emitted");
                        return Some(Token::LParen);
                    },

                    // Closing paren
                    b')' => {
                        // Just emit it
                        println!("Token::RParen emitted");
                        return Some(Token::RParen);
                    },

                    b'$' => {
                        println!("Token::Dollar emitted");
                        return Some(Token::Dollar);
                    },

                    b'#' => {
                        println!("Token::Hash emitted");
                        return Some(Token::Hash);
                    },

                    b'=' => {
                        println!("Token::Equals emitted");
                        return Some(Token::Equals);
                    },

                    b',' => {
                        println!("Token::Comma emitted");
                        return Some(Token::Comma);
                    },

                    // Skip spaces
                    b' ' => {},

                    b @ _ => {
                        panic!("Invalid or unhandled byte {:?} encountered",
                               b);
                    }

                },

                Mode::Newline => match byte {
                    b'\n' => {
                    }
                    _ => panic!("CR without corresponding LF in input file")
                },

                // We've already peeked at byte if we're here
                // That means we know we have something!
                Mode::Text => match peeked {
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
                                // Push *current* (not peeked!) byte
                                buf.push(byte);
                                // Carry on
                            },

                            // Valid varname ends
                            _ => {
                                // Emit text token
                                mode = Mode::None;
                                emit_token_text!(buf);
                            }
                    },

                    None => {
                        // Well, let's go back to normal mode?
                        // Also emit text token
                        mode = Mode::None;
                        emit_token_text!(buf);
                    }
                },

                Mode::Number => match peeked {
                    Some(&peek_byte) => match peek_byte {
                            b'0' ... b'9' => {
                                // Push current byte
                                buf.push(byte);
                            },

                            // Valid number ends
                            _ => {
                                // Emit number token
                                mode = Mode::None;
                                emit_token_number!(buf);
                            }
                    },

                    None => {
                        // Also emit number token
                        mode = Mode::None;
                        emit_token_number!(buf);
                    }
                }

            }
                            
        }
    }
}
