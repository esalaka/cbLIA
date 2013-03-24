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
		return 0;
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
	while (accept(TOKEN_COMMA)) {
		next_token();
		if (variable_definition()) {
		}
		else {
			fprintf(stderr, "Expected variable\n(Got %s at token %d)\n", current->data, current_number);
			return 0;
		}
	}
	
	return 1;
}

static int separated_variables(void)
{
	if (variable_definition()) {
	}
	else {
		return 1;
	}

	if (comma_prefixed_variables()) {
		return 1;
	}
	else {
		fprintf(stderr, "Expected more variables\n(Got %s at token %d)\n", current->data, current_number);
		return 0;
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
		return 1;
	}
}

static int line(void)
{
	if (function_definition()) {
	}
	else {
		fprintf(stderr, "Expected function definition\n(Got %s at token %d)\n", current->data, current_number);
		return 0;
	}

	if (accept(TOKEN_EOL)) {
		return 1;
	}
	else {
		fprintf(stderr, "Expected EOL\n(Got %s at token %d)\n", current->data, current_number);
		return 0;
	}
}

static int definitions(void)
{
	if (line()) {
		while (line()) {
		}
		return 1;
	}
	else {
		fprintf(stderr, "Expected line\n(Got %s at token %d)\n", current->data, current_number);
		return 0;
	}
		
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
	return (ret == 1) ? 0 : 1;
}
