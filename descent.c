#include "descent.h"

/*
 * NOTE: Errors need to propagate up the stack somehow. Make
 * getsym equiv. check for an error flag and return when
 * it's set. Getsym should be a macro to make this possible.
 * Error should set the error flag. Every function should
 * affect the parse tree in such a way that it can be simply
 * freed in lill_descend without memleaks.
 */

/*
 * With current makefile settings, lill_descend will not
 * compile rn. (Unused arguments). THIS IS CORRECT. It
 * should not compile yet because it does nothing. The rest
 * of the program has been proven to compile without this
 * particular function.
 */

int lill_descend(struct lill_token *token_str, int token_count)
{
	return 0;
}
