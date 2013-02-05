#include <stdio.h>
#include <stdlib.h>

#include "tokeniser.h"

#define LERR_NO_INPUT 1

int main(int argc, char *argv[])
{
	const char * default_file = "cblia.lill";
	const char * filename = (argc < 2) ? default_file : argv[1];

	FILE *input;
	int parse_output;
	struct lill_token *token_str;

	/* 
	 * That's just kind of magic up there tho
	 * It's used because there shouldn't be a case where it errors
	 */

	input = fopen(filename, "rb");

	if (input == NULL) {
		fprintf(stdout, "Unable to open %s\n", filename);
		return LERR_NO_INPUT;
	}
	
	/* token_str will be a malloc'd array containing the tokens! */
	parse_output = lill_tokenise(input, &token_str);
	
	switch (parse_output) {
		case LILL_PARSE_UNEXPECTED_CHARACTER:
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

		/* We don't have next phase though so let's just */
		free(token_str);
	}

	/* Finally */
	fclose(input);

	/* Below zero is error */
	return (parse_output < 0) ? 1 : 0;
}

