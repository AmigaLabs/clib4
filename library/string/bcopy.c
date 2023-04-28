/*
 * $Id: string_bcopy.c,v 1.5 2022-03-29 12:04:26 clib2devs Exp $
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

    if (__optimizedCPUFunctions) {
        /* Check if we have altivec enabled */
        if (__getclib2()->hasAltivec) {
            vec_bcopy(src, dest, len);
        }
        else {
            switch (__getclib2()->cpufamily) {
                default:
                    /* Fallback to standard function */
                    memmove(dest, src, len);
            }
        }
    }
    else {
        /* Fallback to standard function */
        memmove(dest, src, len);
    }

    LEAVE();
}
