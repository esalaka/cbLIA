#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "tokeniser.h"
#include "lill_token.h"

#define STREAM_SIZE_INCREMENT 10

int lill_tokenise(FILE *input, struct lill_token **token_str)
{
	size_t new_size, str_size, str_ptr;
	struct lill_token *temp;
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

	str_size = 10;
	str_ptr = 0;
	(*token_str) = calloc(str_size, sizeof(struct lill_token));

	for (curr = '\0'; curr != EOF && mode != MODE_ERROR; curr = fgetc(input)) {
		if (str_ptr == str_size) {
			/* Old values in temporaries */
			new_size = str_size + STREAM_SIZE_INCREMENT;
			temp = (*token_str);

			/* Allocate new memory and copy */
			(*token_str) = calloc(new_size, sizeof(struct lill_token));
			memcpy((*token_str), temp, str_size * sizeof(struct lill_token));

			/* Finally */
			free(temp);

			/* Memory reallocated */
		}

		if (curr == EOF) {
			mode = MODE_ERROR;
			err = 0; /* EOF, no error */
		}

		switch (mode) { /* State machine! */
			case MODE_ERROR:
				break; /* just jump out */

			default:
				if (curr == '\r') {
					curr = fgetc(input);
					if (curr != '\n') {
						mode = MODE_ERROR;
						err = LILL_PARSE_UNEXPECTED_CHARACTER;
						break;
					}
					/* No else needed since we break */

					(*token_str)[str_ptr].type = TOKEN_EOL;
					memcpy((*token_str)[str_ptr].data, "\r\n", 3);
					printf("TOKEN_EOL");
					break;
				}

				/* If nothing else matches... */
				mode = MODE_ERROR;
				err = LILL_PARSE_UNEXPECTED_CHARACTER;
				break;
		}

	}

	if (err != 0) {
		/* So we can just fail */
		free((*token_str));
	}

	return err;
}
