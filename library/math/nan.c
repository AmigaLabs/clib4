/*
 * $Id: math_nan.c,v 1.4 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

static __inline
int digittoint(int c) {
    if ('0' <= c && c <= '9')
        return (c - '0');
    else if ('A' <= c && c <= 'F')
        return (c - 'A' + 10);
    else if ('a' <= c && c <= 'f')
        return (c - 'a' + 10);
    return 0;
}

static void
__scan_nan(uint32_t *words, int num_words, const char *s) {
    int si;        /* index into s */
    int bitpos;    /* index into words (in bits) */

    /* Allow a leading '0x'. (It's expected, but redundant.) */
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        s += 2;

    /* Scan forwards in the string, looking for the end of the sequence. */
    for (si = 0; isxdigit(s[si]); si++);

    /* Scan backwards, filling in the bits in words[] as we go. */
    for (bitpos = 32 * num_words - 4; bitpos >= 0; bitpos -= 4) {
        if (--si < 0)
            break;
        words[bitpos / 32] |= digittoint(s[si]) << (bitpos % 32);
    }
}

double
nan(const char *s) {
    union {
        double d;
        uint32_t bits[2];
    } u = { .bits = { 0x7ff80000, 0 } };

    __scan_nan(u.bits, 2, s);
    return (u.d);
}
