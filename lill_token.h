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
		TOKEN_EOL,      /* \r\n */
                TOKEN_EOF,      /* EOF */
		/* More precise types follow */
		TOKEN_VARIABLE,
		TOKEN_KW_FUNCTION,
		TOKEN_KW_AS,
		TOKEN_KW_INTEGER,
		TOKEN_KW_FLOAT,
		TOKEN_KW_STRING,
		TOKEN_KW_SHORT,
		TOKEN_KEYWORD
	} type; 

	/*
	 * If this weren't auto-allocated we'd run into issues:
	 * Pointers to malloc'd shit inside malloc'd shit
	 */
	char data[LILL_TOKEN_DATA_SIZE];
};

#endif /* CBLIA_LILL_TOKEN_H */
