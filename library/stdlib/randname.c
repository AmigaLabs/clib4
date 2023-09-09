/*
 * $Id: stdlib_randname.c,v 1.0 2023-07-13 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/* This assumes that a check for the template size has already been made */
char *__randname(char *template) {
    int i;
    struct timespec ts;
    unsigned long r;
    struct _clib2 *__clib2 = __CLIB2;

    clock_gettime(CLOCK_REALTIME, &ts);
    r = ts.tv_sec + ts.tv_nsec + __clib2->processId * 65537UL;
    for (i = 0; i < 6; i++, r >>= 5)
        template[i] = 'A' + (r & 15) + (r & 16) * 2;

    return template;
}