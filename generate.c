#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generate.h"
#include "lill_tree_node.h"

struct lill_function_definition
{
    char name[64];
};

const char* lill_tree_node_type_names[] = {
    /* TERMINALS */
    /* THESE MUST MATCH lill_token_type!!!!!!!! */
    "NODE_INVALID",
    "NODE_LEAF_NUMBER",
    "NODE_LEAF_EQUALS",
    "NODE_LEAF_LBRACKET",
    "NODE_LEAF_RBRACKET",
    "NODE_LEAF_COMMA",
    "NODE_LEAF_DOLLAR",
    "NODE_LEAF_HASH",
    "NODE_LEAF_EOL",
    "NODE_LEAF_EOF",

    "NODE_LEAF_VARIABLE",
    "NODE_LEAF_KW_FUNCTION",
    "NODE_LEAF_KW_AS",
    "NODE_LEAF_KW_INTEGER",
    "NODE_LEAF_KW_FLOAT",
    "NODE_LEAF_KW_STRING",
    "NODE_LEAF_KW_SHORT",
    "NODE_LEAF_KEYWORD",

    /* NONTERMINALS */
    "NODE_DEFINITIONS",
    "NODE_LINE",
    "NODE_FUNCTION_DEFINITION",
    "NODE_PARAMETER_LIST",
    "NODE_SEPARATED_VARIABLES",
    "NODE_VARIABLE_DEFINITION",
    "NODE_TYPE_SPECIFICATION",
    "NODE_LONG_TYPE_SPECIFICATION",
    "NODE_SHORT_TYPE_SPECIFICATION",
    "NODE_TYPE_NAME",
    "NODE_COMMA_PREFIXED_VARIABLES"
};

void printTree(struct lill_tree_node *curr, int depth)
{
    unsigned i;
    int j;
 
    for (j = 0; j < depth; ++j) {
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
    }
    putchar('`');
    putchar('-');
    putchar('-');

    printf("%s, %s\n", lill_tree_node_type_names[curr->type], curr->data);

    if (curr->child_count == 0) { return; }

    for (i = 0; i < curr->child_count; ++i) {
        for (j = 0; j < depth + 1; ++j) {
            putchar(' ');
            putchar(' ');
            putchar(' ');
            putchar(' ');
        }
        putchar('|');
        putchar('\n');

        printTree(curr->children[i], depth + 1);
    }
}

void buildFunctionDefinitions(struct lill_tree_node *curr, struct lill_function_definition **defs)
{
    struct lill_function_definition *currdef;
    currdef = *defs;
    struct lill_tree_node *parent;

    if (curr->type != NODE_DEFINITIONS)
    {
        printf ("WTF are we, %s?\n", lill_tree_node_type_names[curr->type]);
        return; /* Error here */
    }
    
    curr = curr->children[0];
    if (curr->type == NODE_LINE) {
        curr = curr->children[0];
        if (curr->type == NODE_FUNCTION_DEFINITION)
        {
            parent = curr;
            curr = curr->children[0];
            if (curr->type == NODE_LEAF_KW_FUNCTION)
            {
                curr = parent->children[1];
                
                if (curr->type == NODE_LEAF_VARIABLE)
                {
                    strncpy(currdef->name, curr->data, sizeof(currdef->name));
                }
            }
        }
    }
}   

void generate_code(const char *file_stem, struct lill_tree_node *tree)
{
    struct lill_tree_node *curr;
    struct lill_function_definition *defs;

    FILE * outfile_coolbasic;
    FILE * outfile_c;

    /* 264 from 260 + 1 + 3, see main.c */
    /* 1 = null terminator, 3 = .cb */
    /* TODO: define a constant for these */
    char filename_coolbasic[264];
    char filename_c[263];

    size_t i;

    /* Empty definition list */
    defs = calloc(256, sizeof(struct lill_tree_node));

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


    /* Just use the damn tree printer and recurse the same way
     * you know what to do
     * it says what to do RIGHT THERE */
    buildFunctionDefinitions(curr, &defs);

    printTree(curr, 0);

    /*
     * TODO: recurse through tree until hit function_definition
     *
     * When that happens, find:
     *      - NODE_TERMINAL_VARIABLE ---> function name
     *      - NODE_SHORT_TYPE_SPECIFICATION ---> type
     *      - NODE_SEPARATED_VARIABLES 
     *          - NODE_VARIABLE_DEFINITION ---> 1st param
     *          - NODE_COMMA_PREFIXED_VARIABLES
     *              - NODE_VARIABLE_DEFINITION ---> rest of params
     *
     * and for each NODE_VARIABLE_DEFINITION:
     *      - NODE_TERMINAL_VARIABLE ---> parameter name
     *      - NODE_TYPE_SPECIFICATION ---> parameter type
     */

    fclose(outfile_coolbasic);
    fclose(outfile_c);
}
