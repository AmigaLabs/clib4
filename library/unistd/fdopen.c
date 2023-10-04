/*
 * $Id: unistd_fdopen.c,v 1.7 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

FILE *
fdopen(int file_descriptor, const char *type) {
    FILE *result = NULL;
    int slot_number;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWSTRING(type);

    assert(type != NULL);

    __check_abort();

    __stdio_lock(__clib4);

    if (type == NULL) {
        SHOWMSG("invalid type parameter");

        __set_errno(EFAULT);
        goto out;
    }

    slot_number = __find_vacant_iob_entry(__clib4);
    if (slot_number < 0) {
        if (__grow_iob_table(__clib4, 0) < 0) {
            SHOWMSG("not enough memory for a file buffer slot");
            goto out;
        }

        slot_number = __find_vacant_iob_entry(__clib4);
        assert(slot_number >= 0);
    }

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    if (__open_iob(__clib4, NULL, type, file_descriptor, slot_number) < 0) {
        SHOWMSG("couldn't open the file for the file descriptor");
        goto out;
    }

    result = (FILE *) __clib4->__iob[slot_number];

out:

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
