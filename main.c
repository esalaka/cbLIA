#include <stdio.h>

#include "tokeniser.h"

#define LERR_NO_INPUT 1

int main(int argc, char *argv[])
{
	const char * default_file = "cblia.lill";
	const char * filename = (argc < 2) ? default_file : argv[1];

	FILE *input;
	int parse_error;
	/* 
	 * That's just kind of magic up there tho
	 * It's used because there shouldn't be a case where it errors
	 */

	input = fopen(filename, "rb");

	if (input == NULL) {
		fprintf(stdout, "Unable to open %s\n", filename);
		return LERR_NO_INPUT;
	}
	
	parse_error = lill_tokenise(input);
	
	switch (parse_error) {
		case LILL_PARSE_UNEXPECTED_CHARACTER:
			fprintf(stderr, "Tokeniser: unexpected character\n");
			break;
	}

	if (parse_error == 0) {
		/* Next phase */
	}

	/* Finally */
	fclose(input);

	return parse_error;
}

