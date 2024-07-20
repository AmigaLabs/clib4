/*
 * $Id: stdio_remove.c,v 1.6 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
remove(const char *filename) {
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(filename);

    assert(filename != NULL);

    __check_abort_f(__clib4);

    if (filename == NULL) {
        SHOWMSG("invalid path name");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        result = unlink(filename);
    } else {
        LONG status;

        D(("trying to delete '%s'", filename));

        status = Delete((STRPTR) filename);
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
