#include <stdio.h>
#include <stdlib.h>

#include "descent.h"
#include "lill_token.h"
#include "lill_tree_node.h"

static int token_total;
static struct lill_token *current;
static int current_number;

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
        return (current->type == type) ? 1 : 0;
}

static int type_name(void)
{
        if (accept(TOKEN_KW_INTEGER)) {
                next_token();
                return 1;
        }
        else if (accept(TOKEN_KW_FLOAT)) {
                next_token();
                return 1;
        }
        else if (accept(TOKEN_KW_STRING)) {
                next_token();
                return 1;
        }
        else if (accept(TOKEN_KW_SHORT)) {
                next_token();
                return 1;
        }
        else {
                fprintf(stderr, "Expected type name\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }
}

static int long_type_specification(void)
{
        if (accept(TOKEN_KW_AS)) {
                next_token();
        }
        else {
                return 1; /* Empty */
        }

        if (type_name()) {
                return 1;
        }
        else {
                /* To avoid spoiling the state */
                /* This is a necessary hack, I guess */
                /* I could avoid it by avoiding optionals */
                previous_token();
                return 0;
        }
}

static int type_specification(void)
{
        if (accept(TOKEN_HASH)) {
                next_token();
                return 1;
        }
        else if (accept(TOKEN_DOLLAR)) {
                next_token();
                return 1;
        }
        else if (long_type_specification()) {
                return 1;
        }
        else {
                fprintf(stderr, "Expected type\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }
}

static int variable_definition(void)
{
        if (accept(TOKEN_VARIABLE)) {
                next_token();
        }
        else {
                fprintf(stderr, "Expected variable\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }

        if (type_specification()) {
                return 1;
        }
        else {
                return 1;
        }

}

static int comma_prefixed_variables(void)
{
        /* No commas just means this is empty */
        if (accept(TOKEN_COMMA)) {
                do {
                        next_token();
                        if (variable_definition()) {
                        }
                        else {
                                fprintf(stderr, "Expected variable\n(Got %s at token %d)\n", current->data, current_number);
                                return 0;
                        }
                } while (accept(TOKEN_COMMA));
        }
        
        return 1;
}

static int separated_variables(void)
{
        if (variable_definition()) {
                if (comma_prefixed_variables()) {
                        return 1;
                }
                else {
                        fprintf(stderr, "Expected more variables\n(Got %s at token %d)\n", current->data, current_number);
                        return 0;
                }
        }
        else {
                return 1;
        }

}

static int parameter_list(void)
{
        if (accept(TOKEN_LBRACKET)) {
                next_token();
        }
        else {
                fprintf(stderr, "Expected opening bracket\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }
        
        if (separated_variables()) {
        }
        else {
                fprintf(stderr, "Expected comma-separated variables\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }

        if (accept(TOKEN_RBRACKET)) {
                next_token();
                return 1;
        }
        else {
                fprintf(stderr, "Expected closing bracket\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }

}

static int function_definition(void)
{
        if (accept(TOKEN_KW_FUNCTION)) {
                next_token();
        }
        else {
                fprintf(stderr, "Expected keyword Function\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }

        if (accept(TOKEN_VARIABLE)) {
                next_token();
        }
        else {
                fprintf(stderr, "Expected variable\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }

        if (parameter_list()) {
        }
        else {
                fprintf(stderr, "Expected parameter list\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }

        if (long_type_specification()) {
                return 1;
        }
        else {
                fprintf(stderr, "Expected long type specification\n(Got %s at token %d)\n", current->data, current_number);
                return 0;
        }
}

static int line(void)
{

        if (accept(TOKEN_EOL)) {
                next_token();
                return 1;
        }
        else {
                if (function_definition()) {
                        return 1;

                        if (accept(TOKEN_EOL)) {
                                next_token();
                                return 1;
                        }
                        else {
                                fprintf(stderr, "Expected EOL\n(Got %s at token %d)\n", current->data, current_number);
                        }
                }
                else {
                        fprintf(stderr, "Expected function definition or EOL\n(Got %s at token %d)\n", current->data, current_number);
                        return 0;
                }
        }
}

static int definitions(void)
{
        while(line()) {
                if (accept(TOKEN_EOF)) {
                        return 1; // We're done
                }
        }

        fprintf(stderr, "Expected line or EOF\n(Got %s at token %d)\n", current->data, current_number);
        return 0;
                
}

int lill_descend(struct lill_token *token_str, int token_count, struct lill_tree_node **tree)
{
        int ret;

        token_total = token_count;
        current_number = 1;
        current = token_str;

        /* Create tree */
        (*tree) = calloc(token_count, sizeof(struct lill_tree_node));

        ret = definitions();
        fprintf(stdout, "Parser returned %d\n", ret);

        if (ret == 1) {
                return 0;
        }
        else {
                /* Rerr */
                free(*tree);
                return 1;
        }
}
