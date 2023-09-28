/*
 * $Id: stdlib_fcvt.c,v 1.0 2022-08-22 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *fcvt(double x, int n, int *dp, int *sign) {
    char tmp[1500] = {0};
    int i, lz;

    if (n > 1400U) n = 1400;
    sprintf(tmp, "%.*f", n, x);
    i = (tmp[0] == '-');
    if (tmp[i] == '0') lz = strspn(tmp + i + 2, "0");
    else lz = -(int) strcspn(tmp + i, ".");

    if (n <= lz) {
        *sign = i;
        *dp = 1;
        if (n > 14U) n = 14;
        return (char *) "000000000000000" + 14 - n;
    }

    return ecvt(x, n - lz, dp, sign);
}
