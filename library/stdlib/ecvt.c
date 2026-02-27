/*
 * $Id: stdlib_ecvt.c,v 1.0 2022-08-22 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
ecvt(double x, int n, int *dp, int *sign) {
    static char buf[16] = {0};
    char tmp[32] = {0};
    int i, j;

    if (n - 1U > 15) n = 15;
    sprintf(tmp, "%.*e", n - 1, x);
    i = *sign = (tmp[0] == '-');
    for (j = 0; tmp[i] != 'e'; j += (tmp[i++] != '.'))
        buf[j] = tmp[i];
    buf[j] = 0;
    *dp = atoi(tmp + i + 1) + 1;

    return buf;
}