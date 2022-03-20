/*
 * $Id: stdlib_lltoa.c,v 1.0 2022-02-11 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H

#include "stdlib_headers.h"

#endif /* _STDLIB_HEADERS_H */

#include <math.h>

char *
lltoa(int64_t value, char *string, int radix) {
    char tmp[65] = {0};
    char *tp = tmp;
    int i;
    unsigned long long v;
    int sign;
    char *sp;

    if (radix > 36 || radix <= 1) {
        errno = EDOM;
        return NULL;
    }

    if (radix == 10 && value < 0LL) {
        sign = 1;
        v = -value;
    } else {
        sign = 0;
        v = (unsigned long long) value;
    }

    while (v || tp == tmp) {
        i = v % (long long) radix;
        v = v / (long long) radix;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i - 10 + 'a';
    }

    if (string == NULL)
        if (!(string = (char *) malloc(tmp - tp + sign + 1)))
            return NULL;
    sp = string;

    if (sign)
        *sp++ = '-';
    while (tp > tmp)
        *sp++ = *--tp;
    *sp = '\0';

    return string;
}