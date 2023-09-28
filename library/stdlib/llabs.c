/*
 * $Id: stdlib_llabs.c,v 1.2 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

long long
llabs(long long x) {
    long long result;

    ENTER();
    SHOWVALUE(x);

    result = (x < 0) ? (-x) : x;

    RETURN(result);
    return (result);
}
