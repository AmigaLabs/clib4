/*
 * $Id: stdio_findvacantiobentry.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef NDEBUG

BOOL __is_valid_iob(struct _clib4 *__clib4, struct iob *iob) {
    BOOL result = FALSE;

    if (iob == NULL) {
        return result;
    }

    if (FLAG_IS_SET(iob->iob_Flags, IOBF_INTERNAL)) {
        /* This is used by vsprintf(), etc. */
        result = TRUE;
    } else if (FLAG_IS_SET(iob->iob_Flags, IOBF_IN_USE)) {
        /* Any in-use iob is valid — covers static __sf[] streams,
           glue-list entries from __sfp(), and legacy __iob[] entries. */
        result = TRUE;
    }

    return result;
}

#endif /* NDEBUG */

/* __find_vacant_iob_entry() removed — all new streams are now allocated
   via __sfp() on the glue list, so the __iob[] scan is dead code. */
