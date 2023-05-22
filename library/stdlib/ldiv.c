/*
 * $Id: stdlib_ldiv.c,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

ldiv_t
ldiv(long n, long d) {
    ldiv_t result;

    ENTER();
    SHOWVALUE(n);
    SHOWVALUE(d);

    assert(d != 0);

    result.quot = n / d;
    result.rem = n % d;

    LEAVE();
    return (result);
}
