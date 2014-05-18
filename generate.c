#include <stdio.h>
#include <string.h>

#include "generate.h"
#include "lill_tree_node.h"

struct lill_function_definition
{
        char name[64];
};

void generate_code(const char *file_stem, struct lill_tree_node *tree)
{
        struct lill_tree_node *curr;

        FILE * outfile_coolbasic;
        FILE * outfile_c;

        /* 264 from 260 + 1 + 3, see main.c */
        /* 1 = null terminator, 3 = .cb */
        /* TODO: define a constant for these */
        char filename_coolbasic[264];
        char filename_c[263];

        size_t i;

        /* construct .cb filename */
        memset(filename_coolbasic, 0, sizeof(filename_coolbasic));
        strncpy(filename_coolbasic, file_stem, 260);

        for (i = 0; filename_coolbasic[i] != '\0'; ++i);
        strncpy(filename_coolbasic + i, ".cb", 3 + 1);
        
        /* construct .c filename */
        memset(filename_c, 0, sizeof(filename_c));
        strncpy(filename_c, file_stem, 260);

        for (i = 0; filename_c[i] != '\0'; ++i);
        strncpy(filename_c + i, ".c", 2 + 1);
        
        /* No 'b' - we're doing Windows text I/O */
        outfile_coolbasic = fopen(filename_coolbasic, "w");
        outfile_c = fopen(filename_c, "w");

        printf("%s, %s\n", filename_coolbasic, filename_c);

        curr = tree;
        /* Placeholder code, will be removed */
        if (curr){}

        /*
         * TODO: recurse through tree until hit function_definition
         *
         * When that happens, find:
         *      - NODE_TERMINAL_VARIABLE ---> function name
         *      - NODE_SHORT_TYPE_SPECIFICATION ---> type
         *      - NODE_SEPARATED_VARIABLES 
         *              - NODE_VARIABLE_DEFINITION ---> 1st param
         *              - NODE_COMMA_PREFIXED_VARIABLES
         *                      - NODE_VARIABLE_DEFINITION ---> rest of params
         *
         * and for each NODE_VARIABLE_DEFINITION:
         *      - NODE_TERMINAL_VARIABLE ---> parameter name
         *      - NODE_TYPE_SPECIFICATION ---> parameter type
         */

        fclose(outfile_coolbasic);
        fclose(outfile_c);
}
