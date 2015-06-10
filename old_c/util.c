#include "util.h"

int is_varname_latin(unsigned char c)
{
    return ((c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z'))
        ? 1 : 0;
}

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
