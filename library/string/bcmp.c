/*
 * $Id: string_bcmp.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

int
bcmp(const void *a, const void *b, size_t len) {
    int result;

    assert(a != NULL && b != NULL);
    assert((int) len >= 0);

    result = memcmp(a, b, len);

    return (result);
}
