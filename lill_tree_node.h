#ifndef CBLIA_LILL_TREE_NODE
#define CBLIA_LILL_TREE_NODE

#include "lill_token.h"

#define LILL_NODE_MAX_CHILDREN 8

struct lill_tree_node {
        struct lill_tree_node *parent;
        size_t child_count;
        struct lill_tree_node *children[LILL_NODE_MAX_CHILDREN];
        enum lill_tree_node_type {
                /* TERMINALS */
                /* THESE MUST MATCH lill_token_type!!!!!!!! */
                NODE_INVALID,
                NODE_LEAF_NUMBER,
                NODE_LEAF_EQUALS,
                NODE_LEAF_LBRACKET,
                NODE_LEAF_RBRACKET,
                NODE_LEAF_COMMA,
                NODE_LEAF_DOLLAR,
                NODE_LEAF_HASH,
                NODE_LEAF_EOL,
                NODE_LEAF_EOF,

                NODE_LEAF_VARIABLE,
                NODE_LEAF_KW_FUNCTION,
                NODE_LEAF_KW_AS,
                NODE_LEAF_KW_INTEGER,
                NODE_LEAF_KW_FLOAT,
                NODE_LEAF_KW_STRING,
                NODE_LEAF_KW_SHORT,
                NODE_LEAF_KEYWORD,

                /* NONTERMINALS */
                NODE_DEFINITIONS,
                NODE_LINE,
                NODE_FUNCTION_DEFINITION,
                NODE_PARAMETER_LIST,
                NODE_SEPARATED_VARIABLES,
                NODE_VARIABLE_DEFINITION,
                NODE_TYPE_SPECIFICATION,
                NODE_LONG_TYPE_SPECIFICATION,
                NODE_SHORT_TYPE_SPECIFICATION,
                NODE_TYPE_NAME,
                NODE_COMMA_PREFIXED_VARIABLES
        } type;
        
        /* Same as tokens */
        char data[LILL_TOKEN_DATA_SIZE];
};

#endif /* CBLIA_LILL_TREE_NODE */
