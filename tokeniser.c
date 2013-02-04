#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "tokeniser.h"
#include "lill_token.h"

#define STREAM_SIZE_INCREMENT 10

int is_varname_letter(unsigned char c)
{
	/* From Windows-1252 */
	return ((c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| (c >= 0xC0 && c <= 0xD6)
		|| (c >= 0xD8 && c <= 0xF6)
		|| (c >= 0xF8)) /* It's always at most 0xFF */
		? 1 : 0;
}

int is_varname_legal(unsigned char c)
{
	return (is_varname_letter(c)
		|| (c >= '0' && c <= '9')
		|| (c == '_'))
		? 1 : 0;
}

void resize_if_needed(size_t *str_size, size_t *str_ptr, struct lill_token **token_str) {

	size_t new_size;
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
		printf("Resized array to %ld\n", new_size);
	}
}

void insert_token(struct lill_token **token_str, size_t *str_size, size_t *str_ptr,
	enum lill_token_type type, const void *data, size_t data_size) {

	resize_if_needed(str_size, str_ptr, token_str);

	(*token_str)[(*str_ptr)].type = type;
	memcpy((*token_str)[(*str_ptr)].data, data, data_size);
	++(*str_ptr); /* Increment */
}


int lill_tokenise(FILE *input, struct lill_token **token_str)
{
	size_t str_size, str_ptr, data_ptr;
	int curr, err;
	unsigned char data[LILL_TOKEN_DATA_SIZE];
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

				if (curr == ' ' || curr == '\t') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));
					printf("TOKEN_TEXT\n");

					/* Exit MODE_TEXT */
					mode = MODE_NONE;
					break;
				}

				if (curr == '(') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));
					printf("TOKEN_TEXT\n");

					insert_token(token_str, &str_size, &str_ptr, TOKEN_LBRACKET, "(", 2);
					printf("TOKEN_LBRACKET\n");

					/* Exit MODE_TEXT */
					mode = MODE_NONE;
					break;
				}

				if (curr == ')') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));
					printf("TOKEN_TEXT\n");

					insert_token(token_str, &str_size, &str_ptr, TOKEN_RBRACKET, ")", 2);
					printf("TOKEN_RBRACKET\n");

					/* Exit MODE_TEXT */
					mode = MODE_NONE;
					break;
				}
					
				if (curr == ',') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));
					printf("TOKEN_TEXT\n");

					insert_token(token_str, &str_size, &str_ptr, TOKEN_COMMA, ",", 2);
					printf("TOKEN_COMMA\n");

					/* Exit MODE_TEXT */
					mode = MODE_NONE;
					break;
				}
					
				if (curr == '$') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));
					printf("TOKEN_TEXT\n");

					insert_token(token_str, &str_size, &str_ptr, TOKEN_DOLLAR, "$", 2);
					printf("TOKEN_DOLLAR\n");

					/* Exit MODE_TEXT */
					mode = MODE_NONE;
					break;
				}

				if (curr == '#') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));
					printf("TOKEN_TEXT\n");

					insert_token(token_str, &str_size, &str_ptr, TOKEN_HASH, "#", 2);
					printf("TOKEN_HASH\n");

					/* Exit MODE_TEXT */
					mode = MODE_NONE;
					break;
				}

				if (curr == '\r') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_TEXT, data, sizeof(data));
					printf("TOKEN_TEXT\n");

					curr = fgetc(input);
					if (curr != '\n') {
						mode = MODE_ERROR;
						err = LILL_PARSE_UNEXPECTED_CHARACTER;
						break;
					}
					/* No else needed since we break */

					/* Write token and increment pointer */
					insert_token(token_str, &str_size, &str_ptr, TOKEN_EOL, "\r\n", 3);
					printf("TOKEN_EOL\n");

					break;
				}
				/* If no match and we're still in MODE_TEXT */
				mode = MODE_ERROR;
				err = LILL_PARSE_UNEXPECTED_CHARACTER;
				break;

			case MODE_NUMBER:
				break;

			case MODE_ERROR:
				break; /* just jump out */

			case MODE_NONE:
			default:
				if (curr == '\r') {
					curr = fgetc(input);
					if (curr != '\n') {
						mode = MODE_ERROR;
						err = LILL_PARSE_UNEXPECTED_CHARACTER;
						break;
					}
					/* No else needed since we break */

					/* Write token and increment pointer */
					insert_token(token_str, &str_size, &str_ptr, TOKEN_EOL, "\r\n", 3);
					printf("TOKEN_EOL\n");

					break;
				}

				if (curr == ' ' || curr == '\t') {
					/* Whitespace! */
					break;
				}

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

				if (curr == '(') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_LBRACKET, "(", 2);
					printf("TOKEN_LBRACKET\n");
					break;
				}

				if (curr == ')') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_RBRACKET, ")", 2);
					printf("TOKEN_RBRACKET\n");
					break;
				}
					
				if (curr == ',') {
					insert_token(token_str, &str_size, &str_ptr, TOKEN_COMMA, ",", 2);
					printf("TOKEN_COMMA\n");
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
