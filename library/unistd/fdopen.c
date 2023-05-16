/*
 * $Id: unistd_fdopen.c,v 1.7 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

FILE *
fdopen(int file_descriptor, const char *type) {
    FILE *result = NULL;
    int slot_number;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWSTRING(type);

    assert(type != NULL);

    __check_abort();

    __stdio_lock(__clib2);

    if (type == NULL) {
        SHOWMSG("invalid type parameter");

        __set_errno(EFAULT);
        goto out;
    }

    slot_number = __find_vacant_iob_entry(__clib2);
    if (slot_number < 0) {
        if (__grow_iob_table(__clib2, 0) < 0) {
            SHOWMSG("not enough memory for a file buffer slot");
            goto out;
        }

        slot_number = __find_vacant_iob_entry(__clib2);
        assert(slot_number >= 0);
    }

    assert(file_descriptor >= 0 && file_descriptor < __clib2->__num_fd);
    assert(__clib2->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__clib2->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    if (__open_iob(__clib2, NULL, type, file_descriptor, slot_number) < 0) {
        SHOWMSG("couldn't open the file for the file descriptor");
        goto out;
    }

    result = (FILE *) __clib2->__iob[slot_number];

out:

    __stdio_unlock(__clib2);

    RETURN(result);
    return (result);
}
