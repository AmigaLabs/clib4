/*
 * $Id: stdlib_itoa.c,v 1.0 2022-02-11 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
itoa(int num, char *buffer, int base) {
    char *p = buffer;
    char tmp[33];
    char *tp = tmp;
    unsigned int unum;
    int i;

    ENTER();
    SHOWVALUE(num);
    SHOWPOINTER(buffer);
    SHOWVALUE(base);

    if (base < 2 || base > 36)
        return NULL;

    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        RETURN(buffer);
        return buffer;
    }

    if (num < 0 && base == 10) {
        *p++ = '-';
        unum = (unsigned int)(-(num + 1)) + 1u;
    } else {
        unum = (unsigned int)num;
    }

    while (unum) {
        i = unum % base;
        if (i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i - 10 + 'a';
        unum /= base;
    }

    while (tp > tmp)
        *p++ = *--tp;
    *p = '\0';

    RETURN(buffer);
    return buffer;
}