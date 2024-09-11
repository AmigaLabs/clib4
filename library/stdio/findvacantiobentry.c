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
    } else {
        __stdio_lock(__clib4);

        if (__clib4->__num_iob > 0 && 0 <= iob->iob_SlotNumber && iob->iob_SlotNumber < __clib4->__num_iob && __clib4->__iob[iob->iob_SlotNumber] == iob)
            result = TRUE;

        __stdio_unlock(__clib4);
    }

    return result;
}

#endif /* NDEBUG */

int __find_vacant_iob_entry(struct _clib4 *__clib4) {
    int result = ERROR;
    int i;

    assert(__clib4->__iob != NULL || __clib4->__num_iob == 0);

    for (i = 0; i < __clib4->__num_iob; i++) {
        if (FLAG_IS_CLEAR(__clib4->__iob[i]->iob_Flags, IOBF_IN_USE)) {
            result = i;
            break;
        }
    }

    return (result);
}
