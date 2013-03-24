#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "tokeniser.h"
#include "lill_token.h"
#include "errors.h"

#define STREAM_SIZE_INCREMENT 10

/* Used for magic. Keep up-to-date. */
/* Keep the ending as \r \t and space. It is important. */
/* $ and # are omitted because they are ALWAYS special cases */
static const char * single_char_tokens = "=(),\r\t ";

static int is_varname_letter(unsigned char c)
{
	/* From Windows-1252 */
	return ((c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| (c >= 0xC0 && c <= 0xD6)
		|| (c >= 0xD8 && c <= 0xF6)
		|| (c >= 0xF8)) /* It's always at most 0xFF */
		? 1 : 0;
}

static int is_varname_legal(unsigned char c)
{
	return (is_varname_letter(c)
		|| (c >= '0' && c <= '9')
		|| (c == '_'))
		? 1 : 0;
}

static void resize_if_needed(int *str_size, int *str_ptr, struct lill_token **token_str) {

	int new_size;
	struct lill_token *temp;

	if ((*str_ptr) == (*str_size)) {
		/* Old values in temporaries */
		new_size = (*str_size) + STREAM_SIZE_INCREMENT;
		temp = (*token_str);

		/* Allocate new memory and copy */
		(*token_str) = calloc(new_size, sizeof(struct lill_token));
		memcpy((*token_str), temp, (*str_size) * sizeof(struct lill_token));

		(*str_size) = new_size;

		/* Finally */
		free(temp);

		/* Memory reallocated */
		printf("Resized array to %d\n", new_size);
	}
}

static void insert_token(struct lill_token **token_str, int *str_size, int *str_ptr,
	enum lill_token_type type, const void *data, int data_size) {

	resize_if_needed(str_size, str_ptr, token_str);

	(*token_str)[(*str_ptr)].type = type;
	memcpy((*token_str)[(*str_ptr)].data, data, data_size);
	++(*str_ptr); /* Increment */
}


int lill_tokenise(FILE *input, struct lill_token **token_str)
{
	int str_size, str_ptr, data_ptr;
	int curr, err;
	unsigned char data[LILL_TOKEN_DATA_SIZE];
	enum lill_token_type token_type;
	enum {
		MODE_NONE,
		MODE_TEXT,
		MODE_NUMBER, /* TODO: negative hex???? */
		MODE_SKIP, /* Not in the state machine */
		MODE_ERROR /* Keep this the last mode */
	} mode;

	curr = 0;
	err = 0;
	mode = MODE_NONE;

	str_size = 10;
	str_ptr = 0;
	(*token_str) = calloc(str_size, sizeof(struct lill_token));

	data_ptr = 0;

	/* curr is first char, while not EOF and no errors, read next character */
	for (curr = fgetc(input); curr != EOF && mode != MODE_ERROR; curr = fgetc(input)) {

		if (curr == EOF) {
			mode = MODE_ERROR;
			err = 0; /* EOF, no error */
		}

		switch (mode) { /* State machine! */
			case MODE_TEXT:
				if (is_varname_legal(curr)) {
					data[data_ptr++] = curr;
					break;
				}

				if (curr == '#' || curr == '$') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));

					insert_token(token_str, &str_size, &str_ptr,
						(curr == '#') ? TOKEN_HASH : TOKEN_DOLLAR, &curr, 1);

					/* Ignore further tests */
					mode = MODE_SKIP;
					break;
				}

				/* MAGIC! */
				if (strchr(single_char_tokens, curr)) {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));

					/* Exit MODE_TEXT */
					mode = MODE_NONE;
					break;
				}

				/* If no match and we're still in MODE_TEXT */
				mode = MODE_ERROR;
				err = LILL_UNEXPECTED_CHARACTER;
				break;

			case MODE_NUMBER:
				break;

			case MODE_ERROR:
				break; /* just jump out */

			case MODE_NONE:
			default:
				if (is_varname_letter(curr) || curr == '_') {
					/* Initialise temp data */
					memset(data, '\0', sizeof(data));
					data_ptr = 0;
					
					/* Insert */
					data[data_ptr++] = curr;

					/* Go to text mode */
					mode = MODE_TEXT;
					break;
				}
		}

		if (mode == MODE_SKIP) {
			/* No further checks necessary */
			mode = MODE_NONE;
			continue;
		}

		if (curr == '\r') {
			curr = fgetc(input);
			if (curr != '\n') {
				mode = MODE_ERROR;
				err = LILL_UNEXPECTED_CHARACTER;
				continue;
			}
			/* No else needed since we break */

			/* Write token and increment pointer */
			insert_token(token_str, &str_size, &str_ptr, TOKEN_EOL, "\r\n", 3);

			continue;
		}

		if (curr == ' ' || curr == '\t') {
			/* Whitespace! */
			continue;
		}

		token_type = 0; /* I know it's invalid ssh */
		switch (curr) {
			case '=':
				token_type = TOKEN_EQUALS;
				break;

			case '(':
				token_type = TOKEN_LBRACKET;
				break;

			case ')':
				token_type = TOKEN_RBRACKET;
				break;

			case ',':
				token_type = TOKEN_COMMA;
				break;
			default:
				if (mode == MODE_NONE) {
					/* If nothing else matches... */
					mode = MODE_ERROR;
					err = LILL_UNEXPECTED_CHARACTER;
					break;
				}
		}
			
		if (err == 0 && token_type != 0) {
			/* &curr is a pointer to one char. The struct data is pre-zeroed. */
			insert_token(token_str, &str_size, &str_ptr, token_type, &curr, 1);
		}

	}

	if (err != 0) {
		/* So we can just fail */
		free((*token_str));
	}

	/* Return error if it exists. */
	return (err != 0) ? err : str_ptr;
}
