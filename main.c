#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokeniser.h"
#include "parser.h"
#include "errors.h"
#include "descent.h"
#include "generate.h"

#define LERR_NO_INPUT 1

int main(int argc, char *argv[])
{
        const char * default_file = "cblia.lill";
        const char * filename = (argc < 2) ? default_file : argv[1];

        /* 
         * That's just kind of magic up there tho
         * It's used because there shouldn't be a case where it errors
         */

        char file_stem[261];

        /*
         * Windows only allows up to 260 characters (bytes?) in the total
         * file *path* - this should be sufficient, as other filesystems
         * generally allow for more.
         */

        FILE *input;
        int parse_output;
        struct lill_token *token_str;
        struct lill_tree_node *tree;       

        size_t i;

        memset(file_stem, 0, sizeof(file_stem));
        strncpy(file_stem, filename, 260);

        for (i = sizeof(file_stem) - 1; i > 0; --i) {
                if (file_stem[i] == '.') {
                        /* Null-terminate to remove extension */
                        file_stem[i] = '\0';
                        break;
                }
                else if (
                        file_stem[i] == '/' ||
                        file_stem[i] == '\\' )
                {
                        /* Break at first slash/backslash */
                        break;
                }
        }

        /* 
         * file_stem should now contain a null-terminated string that contains
         * the filename given in without its extension.
         *
         * If there was no extension, nothing should have been done.
         */

        input = fopen(filename, "rb");

        if (input == NULL) {
                fprintf(stdout, "Unable to open %s\n", filename);
                return LERR_NO_INPUT;
        }
        
        /* token_str will be a malloc'd array containing the tokens! */
        parse_output = lill_tokenise(input, &token_str);
        
        switch (parse_output) {
                case LILL_UNEXPECTED_CHARACTER:
                        fprintf(stderr, "Tokeniser: unexpected character "
                                "in input stream\n");
                        break;

                case 0:
                        fprintf(stdout, "Null output from tokeniser\n");
                        free(token_str);
                        break;

                default:
                        fprintf(stdout, "Tokenisation succesful!\n"
                                "(%d tokens)\n", parse_output);
                        break;
        }

        if (parse_output > 0) {
                /* Next phase */
                if (lill_convert_tokens(&token_str, parse_output) == 0)
                {
                        fprintf(stdout, "Token stream converted\n");
                        if (lill_descend(token_str, parse_output, &tree) == 0) {
                                fprintf(stdout, "Parsed succesfully.\n");

                                /* Enter codegen. */
                                /* As soon as the parser outputs a tree. */
                                generate_code(file_stem, tree);

                                free_tree_recursively(tree); /* Whee */
                        }
                }
                free(token_str);
        }

        /* Finally */
        fclose(input);

        /* Below zero is error */
        return (parse_output < 0) ? 1 : 0;
}

