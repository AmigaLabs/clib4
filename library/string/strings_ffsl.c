/*
 * $Id: string_ffsl.c,v 1.0 2022-12-31 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "strings_headers.h"
#endif /* _STRING_HEADERS_H */

#include "atomic.h"

int ffsl(long i) {
    return i ? a_ctz_l(i) + 1 : 0;
}
