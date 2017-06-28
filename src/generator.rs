use std::fs;
use std::io::Write;

use parser;


// The following is entirely just writing out some code
// The 512 you encounter as string length is pretty much just a hack.
// However, there is no real meaningful way to determine string lengths
// in an interface like this. Refrain from using strings.
pub fn coolbasic(root: &parser::Node, mut outfile: fs::File){
    let functions = match root {
        &parser::Node::Root { functions: ref value } => value,
        _ => unreachable!()
    };
        
    outfile.write_all(b"Include \"cbDLL.cb\"\r\n\r\n").unwrap();
    outfile.write_all(b"// Enter name of your DLL here\r\n").unwrap();
    outfile.write_all(b"DLLInit(DLL_NAME_HERE)\r\n").unwrap();

    for function in functions {
        let (name, datatype, arguments) = match function {
            &parser::Node::Function {
                name: ref n,
                datatype: ref d,
                arguments: ref a
            } => (n, d, a),
            _ => unreachable!()
        };

        outfile.write_all(b"\r\n\r\nFunction ").unwrap();
        outfile.write_all(name).unwrap();

        outfile.write_all(match datatype {
            &parser::Datatype::Integer => {
                b""
            },

            &parser::Datatype::Float => {
                b"#"
            },

            &parser::Datatype::String => {
                b"$"
            },

            _ => {
                println!("Unknown datatype encountered!");
                return;
            }
        }).unwrap();

        outfile.write_all(b"(").unwrap();
        
        let mut first = true;
        for argument in arguments {
            let (name, datatype) = match argument {
                &parser::Node::Argument {
                    name: ref n,
                    datatype: ref d
                } => (n, d),
                _ => unreachable!()
            };

            if !first {
                outfile.write_all(b", ").unwrap();
            }
            else {
                first = false;
            }

            outfile.write_all(name).unwrap();

            outfile.write_all(match datatype {
                &parser::Datatype::Integer => {
                    b""
                },

                &parser::Datatype::Float => {
                    b"#"
                },

                &parser::Datatype::String => {
                    b"$"
                },

                _ => {
                    println!("Unknown datatype encountered!");
                    return;
                }

            }).unwrap();
        }

        // Boilerplate, essentially
        outfile.write_all(b")\r\n").unwrap();
        outfile.write_all(b"    _DLL_InitMemBlock()\r\n").unwrap();

        // Let's go back to the argument business
        for argument in arguments {
            let (name, datatype) = match argument {
                &parser::Node::Argument {
                    name: ref n,
                    datatype: ref d
                } => (n, d),
                _ => unreachable!()
            };

            outfile.write_all(match datatype {
                &parser::Datatype::Integer => b"    _DLL_PutInt(",
                &parser::Datatype::Float => b"    _DLL_PutFloat(",
                &parser::Datatype::String => b"    _DLL_PutString(",
                _ => {
                    println!("Unknown datatype encountered!");
                    return;
                }
            }).unwrap();
            outfile.write_all(name).unwrap();
            outfile.write_all(b")\r\n").unwrap();
        }

        
        // Finally
        outfile.write_all(b"\r\n    _DLL_CallDll(\"").unwrap();
        outfile.write_all(name).unwrap();
        outfile.write_all(b"_cbwrap\")\r\n").unwrap();

        outfile.write_all(match datatype {
            &parser::Datatype::Integer => b"    ret = _DLL_GetInt()\r\n",
            &parser::Datatype::Float => b"    ret# = _DLL_GetFloat()\r\n",
            &parser::Datatype::String => b"    ret$ = _DLL_GetString()\r\n",
            _ => {
                println!("Unknown datatype encountered!");
                return;
            }
        }).unwrap();

        outfile.write_all(b"    _DLL_DeleteMemBlock()\r\n").unwrap();
        outfile.write_all(b"    Return ret\r\n").unwrap();
        outfile.write_all(b"EndFunction").unwrap();
    }
}

pub fn c(root: &parser::Node, mut outfile: fs::File) {
    let functions = match root {
        &parser::Node::Root { functions: ref value } => value,
        _ => unreachable!()
    };

    outfile.write_all(b"#include <string.h>\n\n").unwrap();
    //outfile.write_all("#define WIN32_LEAN_AND_MEAN\n").unwrap();
    //outfile.write_all("#include <Windows.h>\n").unwrap();

    for function in functions {
        let (name, datatype, arguments) = match function {
            &parser::Node::Function {
                name: ref n,
                datatype: ref d,
                arguments: ref a
            } => (n, d, a),
            _ => unreachable!()
        };

        outfile.write_all(b"extern __declspec(dllexport) void ").unwrap();
        outfile.write_all(name).unwrap();
        outfile.write_all(b"_cbwrap(char *_data) {\n").unwrap();

        // We're on Win32, we know this shit
        outfile.write_all(match datatype {
            &parser::Datatype::Integer => b"\tint ret;\n",
            &parser::Datatype::Float => b"\tfloat ret;\n",
            &parser::Datatype::String => {
                b"\t// This is probably NOT what you want\n\tchar * ret;\n"
            },
            _ => {
                println!("Unknown datatype encountered!");
                return;
            }
        }).unwrap();
        outfile.write_all(b"\tint len;\n\n").unwrap();

        outfile.write_all(b"\tlen = ((int *)(_data))[0];\n").unwrap();

        let mut offset = "sizeof(int)".to_string();
        for argument in arguments {
            let (name, datatype) = match argument {
                &parser::Node::Argument {
                    name: ref n,
                    datatype: ref d
                } => (n, d),
                _ => unreachable!()
            };
            
            outfile.write_all(match datatype {
                &parser::Datatype::Integer => b"\tint ",
                &parser::Datatype::Float => b"\tfloat ",
                &parser::Datatype::String => b"\tchar *",
                _ => {
                    println!("Unknown datatype encountered!");
                    return;
                }
            }).unwrap();

            outfile.write_all(name).unwrap();
            outfile.write_all(b" = ").unwrap();
            outfile.write_all(match datatype {
                &parser::Datatype::Integer => b"*((int *)(&(",
                &parser::Datatype::Float => b"*((float *)(&(",
                &parser::Datatype::String => b"((char *)(&(",
                _ => {
                    println!("Unknown datatype encountered!");
                    return;
                }
            }).unwrap();

            let temp_offset = match datatype {
                // This needs to be done BEFORE we read the string
                &parser::Datatype::String => {
                    offset.clone() + " + sizeof(int)"
                },
                _ => {
                    offset.clone()
                }
            };

            outfile.write_all(b"_data[").unwrap();
            outfile.write_all(temp_offset.as_bytes()).unwrap();
            outfile.write_all(b"])));\n").unwrap();
            
            offset = offset.clone() + &match datatype {
                &parser::Datatype::Integer => " + sizeof(int)".to_string(),
                &parser::Datatype::Float => " + sizeof(float)".to_string(),
                &parser::Datatype::String => {
                    " + *((int *)(&(_data[".to_string()
                      + &offset + "]))) + sizeof(int)"
                },
                _ => {
                    println!("Unknown datatype encountered!");
                    return;
                }
            }[..];
        }

        outfile.write_all(b"\n\tret = ").unwrap();
        outfile.write_all(name).unwrap();
        outfile.write_all(b"(").unwrap();

        let mut first = true;
        for argument in arguments {
            let name = match argument {
                &parser::Node::Argument {
                    name: ref n,
                    datatype: _
                } => n,
                _ => unreachable!()
            };
            
            if !first {
                outfile.write_all(b", ").unwrap();
            }
            else {
                first = false;
            }

            // Print each argument for call
            outfile.write_all(name).unwrap();
        }

        outfile.write_all(b");\n").unwrap();

        outfile.write_all(b"\tmemset(_data, 0, len);\n").unwrap();
        outfile.write_all(match datatype {
            &parser::Datatype::String => b"\tstrncpy(_data, ret, len);",
            _ => b"\tmemcpy(_data, &ret, sizeof(ret));"
        }).unwrap();
        outfile.write_all(b"\n}\n").unwrap();
    }
}
