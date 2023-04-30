/*
 * $Id: stdio_fopen.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
fopen(const char *filename, const char *mode) {
    FILE *result = NULL;
    int slot_number;

    ENTER();

    SHOWSTRING(filename);
    SHOWSTRING(mode);

    assert(filename != NULL && mode != NULL);

    __check_abort();

    __stdio_lock();

    if (filename == NULL || mode == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    slot_number = __find_vacant_iob_entry();
    if (slot_number < 0) {
        if (__grow_iob_table(0) < 0) {
            SHOWMSG("couldn't find a free file table, and no memory for a new one");
            goto out;
        }

        slot_number = __find_vacant_iob_entry();
        assert(slot_number >= 0);
    }

    if (__open_iob(filename, mode, -1, slot_number) < 0) {
        SHOWMSG("couldn't open the file");
        goto out;
    }

    result = (FILE *) __CLIB2->__iob[slot_number];

out:

    __stdio_unlock();

    RETURN(result);
    return (result);
}
