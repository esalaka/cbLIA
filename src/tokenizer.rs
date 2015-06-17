use std::io::Bytes;
use std::fs::File;
use std::iter;

enum Mode {
    None,
    Text,
    Number,
    Skip,
    Error
}

pub enum Token {
    Text(Vec<u8>),
    Number(i32),
    Equals,
    LParen,
    RParen,
    Comma,
    Dollar,
    Hash,
    EOL
}

pub struct TokenIterator {
    bytes: Bytes<File>
}

impl TokenIterator {
    pub fn new(bytes: Bytes<File>) -> TokenIterator {
        TokenIterator{ bytes: bytes }
    }
}

impl Iterator for TokenIterator {
    type Item = Token;

    // The actual tokenisation happens here
    fn next(&mut self) -> Option<Self::Item> {
        println!("Next");
        loop {
            let next = self.bytes.next();
            let mut mode: Mode = Mode::None;
            let mut tok: Token;
            let mut buf = Vec::<u8>::new();

            // return None ends iteration
            let byte = match next {
                Some(Ok(value)) => value,
                Some(Err(e)) => return None,
                None => return None
            };

            match mode {
                Mode::None => match byte {
                        b',' => {
                            tok = Token::Comma;
                            println!("Token::Comma");
                            return Some(tok);
                        }
                        b'$' => {
                            tok = Token::Dollar;
                            println!("Token::Dollar");
                            return Some(tok);
                        }
                        b'#' => {
                            tok = Token::Hash;
                            println!("Token::Hash");
                            return Some(tok);
                        }
                        _ => println!("Unhandled byte {:?}", byte)
                    },
                Mode::Text => panic!("Unhandled"),
                Mode::Number => panic!("Unhandled"),
                Mode::Skip => {
                    mode = Mode::None;
                    continue
                }
                Mode::Error => panic!("Error mode reached!")
            };
        }
    }
}
