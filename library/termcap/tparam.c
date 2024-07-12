/*
 * $Id: termcap_tparam.c,v 1.1 2024-07-12 18:26:47 clib4devs Exp $
*/

#ifndef  _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif

#include <term.h>

/* Assuming STRING is the value of a termcap string entry
   containing `%' constructs to expand parameters,
   merge in parameter values and store result in block OUTSTRING points to.
   LEN is the length of OUTSTRING.  If more space is needed,
   a block is allocated with `malloc'.

   The value returned is the address of the resulting string.
   This may be OUTSTRING or may be the address of a block got with `malloc'.
   In the latter case, the caller must free the block.

   The fourth and following args to tparam serve as the parameter values.  */

static char *tparam1(const char *string, char *outstring, int len, char *up, char *left, register int *argp);

/* VARARGS 2 */
char *
tparam(const char *string, char *outstring, int len, int arg0, int arg1, int arg2, int arg3) {
    int arg[4];

    arg[0] = arg0;
    arg[1] = arg1;
    arg[2] = arg2;
    arg[3] = arg3;
    return tparam1(string, outstring, len, NULL, NULL, arg);
}

char *
tgoto(const char *string, int hpos, int vpos) {
    int args[2];
    struct _clib4 *__clib4 = __CLIB4;

    if (!string)
        return NULL;

    args[0] = vpos;
    args[1] = hpos;
    return tparam1(string, __clib4->tgoto_buf, 50, __clib4->__UP, __clib4->__BC, args);
}

static char *
tparam1(const char *string, char *outstring, int len, char *up, char *left, register int *argp) {
    register int c;
    const char *p = string;
    register char *op = outstring;
    char *outend;
    int outlen = 0;

    register int tem;
    int *old_argp = argp;
    int doleft = 0;
    int doup = 0;

    outend = outstring + len;

    while (1) {
        /* If the buffer might be too short, make it bigger.  */
        if (op + 5 >= outend) {
            register char *new;
            int offset = op - outstring;

            if (outlen == 0) {
                outlen = len + 40;
                new = (char *) malloc(outlen);
                if (new == NULL) {
                    __set_errno(ENOMEM);
                    return NULL;
                }
                bcopy(outstring, new, offset);
            } else {
                outlen *= 2;
                new = (char *) realloc(outstring, outlen);
                if (new == NULL) {
                    __set_errno(ENOMEM);
                    return NULL;
                }
            }

            op = new + offset;
            outend = new + outlen;
            outstring = new;
        }
        c = *p++;
        if (!c)
            break;
        if (c == '%') {
            c = *p++;
            tem = *argp;
            switch (c) {
                case 'd':        /* %d means output in decimal.  */
                    if (tem < 10)
                        goto onedigit;
                    if (tem < 100)
                        goto twodigit;
                    break;
                case '3':        /* %3 means output in decimal, 3 digits.  */
                    if (tem > 999) {
                        *op++ = tem / 1000 + '0';
                        tem %= 1000;
                    }
                    *op++ = tem / 100 + '0';
                    break;
                case '2':        /* %2 means output in decimal, 2 digits.  */
                twodigit:
                    tem %= 100;
                    *op++ = tem / 10 + '0';
                onedigit:
                    *op++ = tem % 10 + '0';
                    argp++;
                    break;

                case 'C':
                    /* For c-100: print quotient of value by 96, if nonzero,
                   then do like %+.  */
                    if (tem >= 96) {
                        *op++ = tem / 96;
                        tem %= 96;
                    }
                    break;
                case '+':        /* %+x means add character code of char x.  */
                    tem += *p++;
                    break;
                case '.':        /* %. means output as character.  */
                    if (left) {
                        /* If want to forbid output of 0 and \n and \t,
                           and this is one of them, increment it.  */
                        while (tem == 0 || tem == '\n' || tem == '\t') {
                            tem++;
                            if (argp == old_argp)
                                doup++, outend -= strlen(up);
                            else
                                doleft++, outend -= strlen(left);
                        }
                    }
                    *op++ = tem ? tem : 0200;
                    break;
                case 'f':        /* %f means discard next arg.  */
                    argp++;
                    break;

                case 'b':        /* %b means back up one arg (and re-use it).  */
                    argp--;
                    break;

                case 'r':        /* %r means interchange following two args.  */
                    argp[0] = argp[1];
                    argp[1] = tem;
                    old_argp++;
                    break;

                case '>':        /* %>xy means if arg is > char code of x, */
                    if (argp[0] > *p++) /* then add char code of y to the arg, */
                        argp[0] += *p;    /* and in any case don't output.  */
                    p++;        /* Leave the arg to be output later.  */
                    break;

                case 'a':        /* %a means arithmetic.  */
                    /* Next character says what operation.
                   Add or subtract either a constant or some other arg.  */
                    /* First following character is + to add or - to subtract
                   or = to assign.  */
                    /* Next following char is 'p' and an arg spec
                   (0100 plus position of that arg relative to this one)
                   or 'c' and a constant stored in a character.  */
                    tem = p[2] & 0177;
                    if (p[1] == 'p')
                        tem = argp[tem - 0100];
                    if (p[0] == '-')
                        argp[0] -= tem;
                    else if (p[0] == '+')
                        argp[0] += tem;
                    else if (p[0] == '*')
                        argp[0] *= tem;
                    else if (p[0] == '/')
                        argp[0] /= tem;
                    else
                        argp[0] = tem;

                    p += 3;
                    break;

                case 'i':        /* %i means add one to arg, */
                    argp[0]++;    /* and leave it to be output later.  */
                    argp[1]++;    /* Increment the following arg, too!  */
                    break;

                case '%':        /* %% means output %; no arg.  */
                    goto ordinary;

                case 'n':        /* %n means xor each of next two args with 140.  */
                    argp[0] ^= 0140;
                    argp[1] ^= 0140;
                    break;

                case 'm':        /* %m means xor each of next two args with 177.  */
                    argp[0] ^= 0177;
                    argp[1] ^= 0177;
                    break;

                case 'B':        /* %B means express arg as BCD char code.  */
                    argp[0] += 6 * (tem / 10);
                    break;

                case 'D':        /* %D means weird Delta Data transformation.  */
                    argp[0] -= 2 * (tem % 16);
                    break;

                default:
                    abort();
            }
        } else
            /* Ordinary character in the argument string.  */
            ordinary:
            *op++ = c;
    }
    *op = 0;
    while (doup-- > 0)
        strcat(op, up);
    while (doleft-- > 0)
        strcat(op, left);
    return outstring;
}
