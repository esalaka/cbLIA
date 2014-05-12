#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "parser.h"
#include "lill_token.h"
#include "errors.h"
#include "util.h"

static const char * a[] = {
        "abs",
        "acos",
        "addtext",
        "after",
        "and",
        "animationheight",
        "animationplaying",
        "animationwidth",
        "as",
        "asc",
        "asin",
        "atan",
        NULL
};

static const char * const b[] = {
        "before",
        "bin",
        "box",
        "boxoverlap",
        "byte",
        NULL
};

static const char * const c[] = {
        "calldll",
        "cameraangle",
        "camerafollow",
        "camerapick",
        "camerax",
        "cameray",
        "case",
        "centertext",
        "chdir",
        "chr",
        "circle",
        "cleararray",
        "clearcollisions",
        "clearkeys",
        "clearmouse",
        "clearobjects",
        "cleartext",
        "clonecameraorientation",
        "clonecameraposition",
        "cloneimage",
        "cloneobject",
        "cloneobjectorientation",
        "cloneobjectposition",
        "closefile",
        "closeinput",
        "cls",
        "clscolor",
        "collisionangle",
        "collisionx",
        "collisiony",
        "color",
        "commandline",
        "const",
        "converttointeger",
        "converttotype",
        "copybox",
        "copyfile",
        "cos",
        "countcollisions",
        "countwords",
        "crc32",
        "currentdir",
        "curveangle",
        "curvevalue",
        NULL
};

static const char * const d[] = {
        "data",
        "date",
        "decrypt",
        "default",
        "defaultmask",
        "defaultvisible",
        "deletefile",
        "deletefont",
        "deleteimage",
        "deletememblock",
        "deleteobject",
        "deletesound",
        "dim",
        "distance",
        "distance2",
        "dot",
        "downkey",
        "drawanimation",
        "drawgame",
        "drawghostimage",
        "drawimage",
        "drawimagebox",
        "drawscreen",
        "drawtoimage",
        "drawtoscreen",
        "drawtoworld",
        NULL
};

static const char * const e[] = {
        "each",
        "editmap",
        "ellipse",
        "else",
        "elseif",
        "encrypt",
        "end",
        "endfunction",
        "endif",
        "endsearch",
        "endselect",
        "eof",
        "errors",
        "escapekey",
        "execute",
        "exit",
        NULL
};

static const char * const f[] = {
        "false",
        "field",
        "fileexists",
        "fileoffset",
        "filesize",
        "findinfile",
        "first",
        "flip",
        "float",
        "for",
        "fps",
        "framelimit",
        "function",
        NULL
};

static const char * const g[] = {
        "getangle",
        "getangle2",
        "getcollision",
        "getexename",
        "getkey",
        "getmap",
        "getmap2",
        "getmouse",
        "getpixel",
        "getpixel2",
        "getrgb",
        "getword",
        "gfxmodeexists",
        "ghostobject",
        "global",
        "gosub",
        "goto",
        "gotosavedlocation",
        NULL
};

static const char * const h[] = {
        "hex",
        "hotspot",
        NULL
};

static const char * const i[] = {
        "if",
        "image",
        "imageheight",
        "imagescollide",
        "imagesoverlap",
        "imagewidth",
        "include",
        "initobjectlist",
        "input",
        "instr",
        "int",
        "integer",
        "isdirectory",
        NULL
};

static const char * const k[] = {
        "keydown",
        "keyhit",
        "keyup",
        NULL
};

static const char * const l[] = {
        "last",
        "left",
        "leftkey",
        "len",
        "line",
        "loadanimimage",
        "loadanimobject",
        "loadfont",
        "loadimage",
        "loadmap",
        "loadobject",
        "loadprogram",
        "loadsound",
        "locate",
        "lock",
        "log",
        "log10",
        "loopobject",
        "lower",
        "lset",
        NULL
};

static const char * const m[] = {
        "makedir",
        "makeemitter",
        "makeerror",
        "makeimage",
        "makemap",
        "makememblock",
        "makeobject",
        "makeobjectfloor",
        "mapheight",
        "mapwidth",
        "maskimage",
        "maskobject",
        "max",
        "memblocksize",
        "memcopy",
        "mid",
        "min",
        "mirrorobject",
        "mod",
        "mousedown",
        "mousehit",
        "mousemovex",
        "mousemovey",
        "mousemovez",
        "mouseup",
        "mousewx",
        "mousewy",
        "mousex",
        "mousey",
        "mousez",
        "movecamera",
        "moveobject",
        NULL
};

static const char * const n[] = {
        "new",
        "next",
        "nextobject",
        "not",
        NULL
};

static const char * const o[] = {
        "objectangle",
        "objectfloat",
        "objectframe",
        "objectinteger",
        "objectlife",
        "objectorder",
        "objectpick",
        "objectpickable",
        "objectplaying",
        "objectrange",
        "objectsight",
        "objectsizex",
        "objectsizey",
        "objectsoverlap",
        "objectstring",
        "objectx",
        "objecty",
        "opentoedit",
        "opentoread",
        "opentowrite",
        "or",
        NULL
};

static const char * const p[] = {
        "paintobject",
        "particleanimation",
        "particleemission",
        "particlemovement",
        "peekbyte",
        "peekfloat",
        "peekint",
        "peekshort",
        "pickcolor",
        "pickedangle",
        "pickedobject",
        "pickedx",
        "pickedy",
        "pickimagecolor",
        "pickimagecolor2",
        "pixelpick",
        "playanimation",
        "playobject",
        "playsound",
        "pointcamera",
        "pointobject",
        "pokebyte",
        "pokefloat",
        "pokeint",
        "pokeshort",
        "positioncamera",
        "positionmouse",
        "positionobject",
        "print",
        "putpixel",
        "putpixel2",
        NULL
};

static const char * const r[] = {
        "rand",
        "randomize",
        "read",
        "readbyte",
        "readfloat",
        "readint",
        "readline",
        "readshort",
        "readstring",
        "redim",
        "repeat",
        "replace",
        "resetobjectcollision",
        "resizeimage",
        "resizememblock",
        "restore",
        "return",
        "right",
        "rightkey",
        "rnd",
        "rotatecamera",
        "rotateimage",
        "rotateobject",
        "rounddown",
        "roundup",
        "rset",
        NULL
};

static const char * const s[] = {
        "safeexit",
        "sar",
        "saveimage",
        "saveprogram",
        "screen",
        "screendepth",
        "screengamma",
        "screenheight",
        "screenpositionobject",
        "screenshot",
        "screenwidth",
        "seekfile",
        "select",
        "setfont",
        "setmap",
        "setsound",
        "settile",
        "setupcollision",
        "setwindow",
        "shl",
        "short",
        "showmouse",
        "showobject",
        "shr",
        "sin",
        "smooth2d",
        "soundplaying",
        "sqrt",
        "startsearch",
        "stopanimation",
        "stopobject",
        "stopsound",
        "str",
        "string",
        "strinsert",
        "strmove",
        "strremove",
        NULL
};

static const char * const t[] = {
        "tan",
        "text",
        "textheight",
        "textwidth",
        "then",
        "time",
        "timer",
        "to",
        "translatecamera",
        "translateobject",
        "trim",
        "true",
        "turncamera",
        "turnobject",
        "type",
        NULL
};

static const char * const u[] = {
        "unlock",
        "until",
        "updategame",
        "upkey",
        "upper",
        NULL
};

static const char * const v[] = {
        "verticaltext",
        NULL
};

static const char * const w[] = {
        "wait",
        "waitkey",
        "waitmouse",
        "wend",
        "while",
        "wrapangle",
        "write",
        "writebyte",
        "writefloat",
        "writeint",
        "writeline",
        "writeshort",
        "writestring",
        NULL
};

static const char * const x[] = {
        "xor",
        NULL
};

static const char * const * const initials[] = {
        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        NULL, /* j */
        k,
        l,
        m,
        n,
        o,
        p,
        NULL, /* q */
        r,
        s,
        t,
        u,
        v,
        w,
        x,
        NULL, /* y */
        NULL /* z */
};
/* Access: initials[char - 'a'] */

static void fixup_keyword(struct lill_token *current)
{
        char temp[LILL_TOKEN_DATA_SIZE];
        int i;

        memset(temp, 0, sizeof(temp));

        for (i = 0; current->data[i] != 0; ++i) {
                temp[i] = tolower(current->data[i]);
        }

        /* I know this looks horrible */
        if (strcmp(temp, "function") == 0) {
                current->type = TOKEN_KW_FUNCTION;
        }
        else if (strcmp(temp, "as") == 0) {
                current->type = TOKEN_KW_AS;
        }
        else if (strcmp(temp, "integer") == 0) {
                current->type = TOKEN_KW_INTEGER;
        }
        else if (strcmp(temp, "float") == 0) {
                current->type = TOKEN_KW_FLOAT;
        }
        else if (strcmp(temp, "string") == 0) {
                current->type = TOKEN_KW_STRING;
        }
        else if (strcmp(temp, "short") == 0) {
                current->type = TOKEN_KW_SHORT;
        }

        /* THERE ACTUALLY IS AN EXPLANATION:
         *
         * Namely, I only need a very limited set of
         * keywords to be recognised. The rest are, in all
         * cases, INVALID. This means that I can simply go
         * and detect whether they're one of these few
         * valid ones and if they aren't, do nothing.
         *
         * The next phase of the parser should kill self if
         * it runs into an instance of TOKEN_KEYWORD.
         */
}

static int detect_keyword(struct lill_token *current)
{
        char temp[LILL_TOKEN_DATA_SIZE];
        int i;

        memset(temp, 0, sizeof(temp));

        for (i = 0; current->data[i] != 0; ++i) {
                temp[i] = tolower(current->data[i]);
        }

        const char * const * const initial = initials[temp[0] - 'a'];

        for (i = 0; initial[i] != NULL; ++i)
        {
                if (strcmp(temp, initial[i]) == 0) {
                        current->type = TOKEN_KEYWORD;
                        fixup_keyword(current);
                        fprintf(stdout, "Keyword %s\n", current->data);
                        return 1;
                }
        }

        return 0;
}

int lill_convert_tokens(struct lill_token **token_str, int token_count)
{
        int i;
        unsigned char initial;

        for (i = 0; i < token_count; ++i) {
                
                if ((*token_str)[i].type == TOKEN_EOF) {
                        fprintf(stdout, "EOF\n");
                        break;
                }

                if ((*token_str)[i].type == TOKEN_TEXT) {
                        initial = (*token_str)[i].data[0];

                        if ((!is_varname_latin(initial)) /* Sorry for the hacky look */
                            || (initials[initial - 'a'] == NULL)
                            || (!detect_keyword((*token_str) + i))) {
                                /* It looks like this to avoid a lot of weird shit */
                                /*
                                 * "If varname doesn't begin with a latin
                                 * character, or if it does but no single
                                 * keyword begins with the initial,
                                 * or the variable name matches no keyword,
                                 * we have a TOKEN_VARIABLE"
                                 */

                                (*token_str)[i].type = TOKEN_VARIABLE;
                                fprintf(stdout, "Variable %s\n", (*token_str)[i].data);
                        }
                }
        }
        
        return 0; /* No error is zero */
        /* I just do things like that */
}
