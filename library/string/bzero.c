/*
 * $Id: string_bzero.c,v 1.5 2022-03-29 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void
bzero(void *m, size_t len) {
    assert((len == 0) || (m != NULL && (int) len > 0));

    ENTER();
    SHOWPOINTER(m);
    SHOWVALUE(len);

    DECLARE_UTILITYBASE();

    if (__global_clib2 != NULL && __global_clib2->optimizedCPUFunctions) {
        /* Check if we have altivec enabled */
        if (__global_clib2->hasAltivec) {
            vec_bzero(m, len);
        }
        else {
            /* Fallback to standard function */
            ClearMem(m, len);
        }
    }
    else {
        /* Fallback to standard function */
        ClearMem(m, len);
    }

    LEAVE();
}
