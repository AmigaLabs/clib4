/*
 * $Id: string_ffsll.c,v 1.0 2022-12-31 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "strings_headers.h"
#endif /* _STRING_HEADERS_H */

#include "atomic.h"

int ffsll(long long i) {
    return i ? a_ctz_64(i)+1 : 0;
}
