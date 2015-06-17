use std::io::Bytes;
use std::fs::File;
use std::iter;

pub enum Token {
    Text(String),
    Number(i32),
    Equals,
    LParen,
    RParen,
    Comma,
    Dollar,
    Hash,
    EOL,
    EOF
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
        let byte = self.bytes.next().unwrap().unwrap();
        print!("{:?}", byte);
        
        match byte {
            // this should be replaced with the actual characters
            70 => Some(Token::Text("a".to_string())),
            _ => None
        }
    }
}
