/*
 * $Id: stdlib_randname.c,v 1.0 2023-07-13 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/* This assumes that a check for the template size has already been made */
char *__randname(char *template) {
    int i;
    struct timespec ts;
    unsigned long r;
    struct _clib4 *__clib4 = __CLIB4;

    clock_gettime(CLOCK_REALTIME, &ts);
    r = ts.tv_sec + ts.tv_nsec + __clib4->processId * 65537UL;
    for (i = 0; i < 6; i++, r >>= 5)
        template[i] = 'A' + (r & 15) + (r & 16) * 2;

    return template;
}