#ifndef CBLIA_TOKENISER_H
#define CBLIA_TOKENISER_H

struct FILE;

/* Errors */
#define LILL_PARSE_UNEXPECTED_CHARACTER 1

/* Functions */
int lill_tokenise(FILE *input);

#endif /* CBLIA_TOKENISER_H */
