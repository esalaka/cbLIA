#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "descent.h"
#include "lill_token.h"
#include "lill_tree_node.h"

/* To avoid accidents */
#define RETURN_WITH_SUCCESS() \
    do {\
        make_parent_current();\
        return 1;\
    } while (0)

#define RETURN_WITH_FAILURE() return 0

#define RETURN_FINISH_PARSE() return 1

static int token_total;
static struct lill_token *current;
static int current_number;

static struct lill_tree_node *current_node;
static struct lill_tree_node *last_node;
static int node_number;
static struct lill_tree_node **tree_ptr;

void free_tree_recursively(struct lill_tree_node *tree)
{
    if (tree != NULL) {
        size_t i;
        for (i = 0; i < tree->child_count; ++i) {
            free_tree_recursively(tree->children[i]);
        }

        /* Finally */
        free(tree);
    }
}

static void new_root_node(enum lill_tree_node_type type)
{
    ++node_number;

    current_node->parent = NULL;
    current_node->child_count = 0;
    memset(current_node->children, 0, sizeof(current_node->children));

    /* No data in internal nodes */
    current_node->type = type;
    memset(current_node->data, 0, sizeof(current_node->data));

    printf("Root node %d\n", type);
}

static void new_leaf_node(void)
{
    last_node = calloc(1, sizeof(struct lill_tree_node));
    ++node_number;

    last_node->parent = current_node;
    last_node->child_count = 0;
    memset(last_node->children, 0, sizeof(last_node->children));

    /* This is why node and token types match! */
    last_node->type = current->type;
    memcpy(last_node->data, current->data, sizeof(current->data));

    current_node->children[current_node->child_count] = last_node;
    ++(current_node->child_count);

    printf("Leaf node %d\n", current->type);
}

static void new_internal_node(enum lill_tree_node_type type)
{
    /* Boilerplate... */
    last_node = calloc(1, sizeof(struct lill_tree_node));
    ++node_number;

    last_node->parent = current_node;
    last_node->child_count = 0;
    memset(last_node->children, 0, sizeof(last_node->children));

    last_node->type = type;
    memset(last_node->data, 0, sizeof(last_node->data));

    current_node->children[current_node->child_count] = last_node;
    ++(current_node->child_count);

    current_node = last_node;

    printf("Internal node %d\n", type);
}

static void make_parent_current(void)
{
    /* Duh */
    current_node = current_node->parent;

    printf("Traversing up\n");
}

static void next_token(void)
{
    if (current_number < token_total)
    {
        ++current_number;
        ++current;
    }
    else
    {
        fprintf(stderr, "All tokens read\n");
        current = NULL;
    }
}

static void previous_token(void)
{
    /* If this crashes the program, I've written my code wrong. */
    --current_number;
    --current;
}

static int accept(enum lill_token_type type)
{
    if (current == NULL) {
        fprintf(stderr, "CRITICAL: current was NULL inside accept\n");
        return 0;
    }
    
    if (current->type == type) {
        new_leaf_node();
        return 1;
    }

    return 0;
}

static int type_name(void)
{
    new_internal_node(NODE_TYPE_NAME);

    if (accept(TOKEN_KW_INTEGER)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else if (accept(TOKEN_KW_FLOAT)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else if (accept(TOKEN_KW_STRING)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else if (accept(TOKEN_KW_SHORT)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else {
        fprintf(stderr, "Expected type name\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }
}

static int long_type_specification(void)
{
    new_internal_node(NODE_LONG_TYPE_SPECIFICATION);

    if (accept(TOKEN_KW_AS)) {
        next_token();
    }
    else {
        RETURN_WITH_SUCCESS(); /* Empty */
    }

    if (type_name()) {
        RETURN_WITH_SUCCESS();
    }
    else {
        /* To avoid spoiling the state */
        /* This is a necessary hack, I guess */
        /* I could avoid it by avoiding optionals */
        previous_token();
        RETURN_WITH_FAILURE();
    }
}

static int short_type_specification(void)
{
    new_internal_node(NODE_SHORT_TYPE_SPECIFICATION);

    if (accept(TOKEN_HASH)) {
        next_token();
    }
    else if (accept(TOKEN_DOLLAR)) {
        next_token();
    }

    RETURN_WITH_SUCCESS();
}

static int type_specification(void)
{
    new_internal_node(NODE_TYPE_SPECIFICATION);

    if (accept(TOKEN_HASH)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else if (accept(TOKEN_DOLLAR)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else if (long_type_specification()) {
        RETURN_WITH_SUCCESS();
    }
    else {
        fprintf(stderr, "Expected type\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }
}

static int variable_definition(void)
{
    new_internal_node(NODE_VARIABLE_DEFINITION);

    if (accept(TOKEN_VARIABLE)) {
        next_token();
    }
    else {
        fprintf(stderr, "Expected variable\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }

    if (type_specification()) {
        RETURN_WITH_SUCCESS();
    }
    else {
        RETURN_WITH_SUCCESS();
    }

}

static int comma_prefixed_variables(void)
{
    new_internal_node(NODE_COMMA_PREFIXED_VARIABLES);

    /* No commas just means this is empty */
    if (accept(TOKEN_COMMA)) {
        do {
            next_token();
            if (variable_definition()) {
            }
            else {
                fprintf(stderr, "Expected variable\n(Got %s at token %d)\n", current->data, current_number);
                RETURN_WITH_FAILURE();
            }
        } while (accept(TOKEN_COMMA));
    }
    
    RETURN_WITH_SUCCESS();
}

static int separated_variables(void)
{
    new_internal_node(NODE_SEPARATED_VARIABLES);

    if (variable_definition()) {
        if (comma_prefixed_variables()) {
            RETURN_WITH_SUCCESS();
        }
        else {
            fprintf(stderr, "Expected more variables\n(Got %s at token %d)\n", current->data, current_number);
            RETURN_WITH_FAILURE();
        }
    }
    else {
        RETURN_WITH_SUCCESS();
    }

}

static int parameter_list(void)
{
    new_internal_node(NODE_PARAMETER_LIST);

    if (accept(TOKEN_LBRACKET)) {
        next_token();
    }
    else {
        fprintf(stderr, "Expected opening bracket\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }
    
    if (separated_variables()) {
    }
    else {
        fprintf(stderr, "Expected comma-separated variables\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }

    if (accept(TOKEN_RBRACKET)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else {
        fprintf(stderr, "Expected closing bracket\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }

}

static int function_definition(void)
{
    new_internal_node(NODE_FUNCTION_DEFINITION);

    if (accept(TOKEN_KW_FUNCTION)) {
        next_token();
    }
    else {
        fprintf(stderr, "Expected keyword Function\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }

    if (accept(TOKEN_VARIABLE)) {
        next_token();
    }
    else {
        fprintf(stderr, "Expected variable\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }

    if (short_type_specification()) {
    }
    else {
        fprintf(stderr, "Expected short type specification\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }

    if (parameter_list()) {
    }
    else {
        fprintf(stderr, "Expected parameter list\n(Got %s at token %d)\n", current->data, current_number);
        RETURN_WITH_FAILURE();
    }

    RETURN_WITH_SUCCESS();
}

static int line(void)
{
    new_internal_node(NODE_LINE);

    if (accept(TOKEN_EOL)) {
        next_token();
        RETURN_WITH_SUCCESS();
    }
    else {
        if (function_definition()) {
            RETURN_WITH_SUCCESS();

            if (accept(TOKEN_EOL)) {
                next_token();
                RETURN_WITH_SUCCESS();
            }
            else {
                fprintf(stderr, "Expected EOL\n(Got %s at token %d)\n", current->data, current_number);
            }
        }
        else {
            fprintf(stderr, "Expected function definition or EOL\n(Got %s at token %d)\n", current->data, current_number);
            RETURN_WITH_FAILURE();
        }
    }
}

static int definitions(void)
{
    /* Create tree */
    new_root_node(NODE_DEFINITIONS);

    /* Current node is:
     *      NULL
     *      0
     *      {}
     *      NODE_DEFINITIONS
     *      ""
     */

    while(line()) {
        if (accept(TOKEN_EOF)) {
            RETURN_FINISH_PARSE(); /* We're done */
        }
    }

    fprintf(stderr, "Expected line or EOF\n(Got %s at token %d)\n", current->data, current_number);
    RETURN_WITH_FAILURE();
        
}

int lill_descend(struct lill_token *token_str, int token_count, struct lill_tree_node **tree)
{
    int ret;

    token_total = token_count;
    current_number = 1; /* Which token are we at */
    current = token_str;

    /* Create tree */
    tree_ptr = tree;
    (*tree_ptr) = calloc(1, sizeof(struct lill_tree_node));
    current_node = *tree_ptr;
    last_node = current_node;
    node_number = 0;

    ret = definitions();
    fprintf(stdout, "Parser returned %d\n", ret);

    if (ret == 1) {
        return 0;
    }
    else {
        /* Rerr */
        free_tree_recursively(*tree);
        return 1;
    }
}
