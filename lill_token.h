#ifndef CBLIA_LILL_TOKEN_H
#define CBLIA_LILL_TOKEN_H

#define LILL_TOKEN_DATA_SIZE 64

struct lill_token {
	enum lill_token_type {
		TOKEN_TEXT,
		TOKEN_NUMBER,
		TOKEN_EQUALS,   /* = */
		TOKEN_LBRACKET, /* ( */
		TOKEN_RBRACKET, /* ) */
		TOKEN_COMMA,    /* , */
		TOKEN_DOLLAR,   /* $ */
		TOKEN_HASH,     /* # */
		TOKEN_EOL       /* \r\n */
	} type; /* I think that's all of them for now? */

	/*
	 * If this weren't auto-allocated we'd run into issues:
	 * Pointers to malloc'd shit inside malloc'd shit
	 */
	unsigned char data[LILL_TOKEN_DATA_SIZE];
};

#endif /* CBLIA_LILL_TOKEN_H */
