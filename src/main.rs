extern crate rustc_serialize;
extern crate docopt;


mod tokenizer;
mod parser;
mod generator;


use std::fs::File;
use std::io::Read;
use docopt::Docopt;


// Usage string
static USAGE: &'static str = "
Usage: cbLIA <source>
";

// Args struct for usage string
#[derive(RustcDecodable)]
struct Args {
    arg_source: String
}


fn main() {
    // unwrap_or_else exits with the "Usage:" error if args are wrong
    let args: Args = Docopt::new(USAGE)
                       .and_then(|d| d.decode())
                       .unwrap_or_else(|e| e.exit());

    // Grab given source file
    let input_filename = args.arg_source;

    // Split it on each .
    let filename_segments = (&input_filename)
                            .split(".")
                            .collect::<Vec<_>>();
    
    // Get all segments except the last one
    let filename_stem = filename_segments
                        .iter()
                        .take(filename_segments.len() - 1)
                        .fold("".to_string(), |acc, item| acc + item);

    // If we have anything left, use that.
    // Otherwise, use the full input filename.
    let base_filename = if filename_stem.len() > 0 {
                            filename_stem
                        }
                        else {
                            (&input_filename).to_string()
                        };
    
    let f = File::open(input_filename.to_owned()).unwrap();

    let tokens = tokenizer::TokenIterator::new(f);

    let ast = match parser::parse(tokens) {
        Ok(node) => node,
        Err(e) => panic!(e)
    };

    let cb_file = File::create((&base_filename).to_string() + ".cb").unwrap();
    let c_file = File::create((&base_filename).to_string() + ".c").unwrap();

    // Write
    generator::coolbasic(&ast, cb_file);
    generator::c(&ast, c_file);

    // Files should be closed when they leave scope.
}
