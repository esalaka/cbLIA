#include <stdio.h>

#include "tokeniser.h"

int lill_tokenise(FILE *input)
{
	int curr, err;
	enum {
		MODE_NONE,
		MODE_TEXT,
		MODE_NUMBER, /* TODO: negative hex???? */
		MODE_ERROR /* Keep this the last mode */
	} mode;

	curr = 0;
	err = 0;
	mode = MODE_NONE;

	for (curr = '\0'; curr != EOF && mode != MODE_ERROR; curr = fgetc(input)) {
		switch (mode) { /* State machine! */
			default:
				mode = MODE_ERROR;
				err = LILL_PARSE_UNEXPECTED_CHARACTER;
				break;
		}
	}

	return err;
}
