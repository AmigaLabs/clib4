/*
 * $Id: unistd_symlink.c,v 1.8 2006-01-29 09:17:00 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
symlink(const char *actual_path, const char *symbolic_path) {
    struct name_translation_info actual_path_name_nti;
    struct name_translation_info symbolic_path_name_nti;
    int result = ERROR;
    LONG status;

    ENTER();

    SHOWSTRING(actual_path);
    SHOWSTRING(symbolic_path);

    assert(actual_path != NULL && symbolic_path != NULL);

    __check_abort();

    if (actual_path == NULL || symbolic_path == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    if (__CLIB2->__unix_path_semantics) {
        if (actual_path[0] == '\0' || symbolic_path[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno(ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&actual_path, &actual_path_name_nti) != 0)
            goto out;

        if (__translate_unix_to_amiga_path_name(&symbolic_path, &symbolic_path_name_nti) != 0)
            goto out;

        if (actual_path_name_nti.is_root || symbolic_path_name_nti.is_root) {
            __set_errno(EACCES);
            goto out;
        }
    }

    SHOWMSG("trying to make that link");

    status = MakeLink((STRPTR) symbolic_path, (APTR) actual_path, LINK_SOFT);
    if (status == DOSFALSE) {
        SHOWMSG("that didn't work");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
