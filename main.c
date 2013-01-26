#include <stdio.h>

#include "parser.h"

#define LERR_NO_INPUT 1

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

	lexer_return = 0;

	if (lexer_return > 0) {
		fprintf(stdout, "Exiting due to lexer error\n");
	}
	
	compiler_return = 0;

	if (compiler_return > 0) {
		fprintf(stdout, "Exiting due to compiler error\n");
	}

	/* Finally */
	fclose(input);

	return 0;
}

