/*
 * $Id: string_bcopy.c,v 1.6 2024-03-22 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void
bcopy(const void *src, void *dest, size_t len) {
    ENTER();
    SHOWPOINTER(src);
    SHOWPOINTER(dest);
    SHOWVALUE(len);

    assert((len == 0) || (src != NULL && dest != NULL && (int) len > 0));

    memmove(dest, src, len);

    LEAVE();
}
