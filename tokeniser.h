#ifndef CBLIA_TOKENISER_H
#define CBLIA_TOKENISER_H

/* Forward declarations */
struct FILE;
struct lill_token;

/* Errors */
#define LILL_PARSE_UNEXPECTED_CHARACTER 1

/* Functions */
int lill_tokenise(FILE *input, struct lill_token **token_str);

#endif /* CBLIA_TOKENISER_H */
