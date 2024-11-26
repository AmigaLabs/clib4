/*
 * $Id: stdio_fopen.c,v 1.6 2024-07-20 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
fopen(const char *filename, const char *mode) {
    FILE *result = NULL;
    int slot_number;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(filename);
    SHOWSTRING(mode);

    assert(filename != NULL && mode != NULL);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    if (filename == NULL || mode == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    slot_number = __find_vacant_iob_entry(__clib4);
    if (slot_number < 0) {
        if (__grow_iob_table(__clib4, 0) < 0) {
            SHOWMSG("couldn't find a free file table, and no memory for a new one");
            goto out;
        }

        slot_number = __find_vacant_iob_entry(__clib4);
        assert(slot_number >= 0);
    }

    if (__open_iob(__clib4, filename, mode, -1, slot_number) < 0) {
        SHOWMSG("couldn't open the file");
        goto out;
    }

    result = (FILE *) __clib4->__iob[slot_number];

out:

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
