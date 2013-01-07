#include <stdio.h>
#include <string.h>

#define LERR_NO_INPUT 1

int lill_parser(FILE *);

int main(int argc, char *argv[])
{
	const char * default_file = "cblia.lill";
	const char * filename = (argc < 2) ? default_file : argv[1];

	FILE *input;
	int parser_return, lexer_return, compiler_return;
	/* 
	 * That's just kind of magic up there tho
	 * It's used because there shouldn't be a case where it errors
	 */

	input = fopen(filename, "rb");

	if (input == NULL) {
		fprintf(stdout, "Unable to open %s\n", filename);
		return LERR_NO_INPUT;
	}

	parser_return = lill_parser(input);

	if (parser_return > 0) {
		fprintf(stdout, "Exiting due to parser error\n");
	}
	/* Finally */
	fclose(input);

	return 0;
}

int lill_parser(FILE *input)
{
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

	if 
	
	return 0;
}
