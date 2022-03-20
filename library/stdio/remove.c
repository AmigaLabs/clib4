/*
 * $Id: stdio_remove.c,v 1.6 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
remove(const char *filename) {
    int result = ERROR;

    ENTER();

    SHOWSTRING(filename);

    assert(filename != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (filename == NULL) {
        SHOWMSG("invalid path name");

        __set_errno(EFAULT);
        goto out;
    }

    if (__global_clib2->__unix_path_semantics) {
        result = unlink(filename);
    } else {
        LONG status;

        D(("trying to delete '%s'", filename));

        status = DeleteFile((STRPTR) filename);
        if (status == DOSFALSE) {
            __set_errno(__translate_access_io_error_to_errno(IoErr()));
            goto out;
        }

        result = OK;
    }

out:

    RETURN(result);
    return (result);
}
