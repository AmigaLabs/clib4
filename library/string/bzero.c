/*
 * $Id: string_bzero.c,v 1.6 2024-03-22 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void
explicit_bzero (void *m, size_t len) {
    memset (m, '\0', len);
    /* Compiler barrier.  */
    asm volatile ("" ::: "memory");
}

void
bzero(void *m, size_t len) {
    assert((len == 0) || (m != NULL && (int) len > 0));

    ENTER();
    SHOWPOINTER(m);
    SHOWVALUE(len);

    DECLARE_UTILITYBASE();

    ClearMem(m, len);

    LEAVE();
}
