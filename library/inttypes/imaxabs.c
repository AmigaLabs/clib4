/*
 * $Id: inttypes_imaxabs.c,v 1.2 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _INTTYPES_HEADERS_H
#include "inttypes_headers.h"
#endif /* _INTTYPES_HEADERS_H */

intmax_t
imaxabs(intmax_t x) {
    intmax_t result;

    ENTER();
    SHOWVALUE(x);

    if (x < 0)
        result = (-x);
    else
        result = x;

    RETURN(result);
    return (result);
}
