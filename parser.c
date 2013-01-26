#include "parser.h"

#include <string.h>

struct lill_def {
	char *whitespace; /* Null-terminated!!! */
} def;

struct lill_state {
	int cur;
	char token[128];
} state;

struct lill_parser_token {
	/* DATA */
	struct lill_parser_token *next;
};

/* We only reserve space for 200 tokens */
/* OTOH this program is basically a hack, it should suffice */
struct lill_token_pool {
	char pool[200*sizeof(struct lill_parser_token)], *pool_pointer;
} pool;

int lill_parser(FILE *input)
{
	
	/* ASCII whitespace only */
	/* Gomen. I'm going the "ASCII or a derivative" route */
	def.whitespace = " \t";

	state.cur = 0;
	memset(state.token, '\0', sizeof(state.token));

	memset(pool.pool, 0, sizeof(pool.pool));
	pool.pool_pointer = pool.pool; /* I'm serious */

	/* PARSE */

	/* Because CB, I'll clear state on each newline */
	/*
	 * This means that before ) or after As \r\n is
	 * always unexpected!
	 */

	state.cur = fgetc(input);

	/* Skip beginning whitespace */
	while (state.cur && strchr(def.whitespace, state.cur))
		state.cur = fgetc(input);
	
	return 0;
}
