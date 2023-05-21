/*
 * $Id: unistd_link.c,v 1.8 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
link(const char *existing_path, const char *new_path) {
    struct name_translation_info existing_path_name_nti;
    struct name_translation_info new_path_name_nti;
    BPTR existing_path_lock = BZERO;
    int result = ERROR;
    LONG status;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWSTRING(existing_path);
    SHOWSTRING(new_path);

    assert(existing_path != NULL && new_path != NULL);

    __check_abort();

    if (existing_path == NULL || new_path == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    if (__clib2->__unix_path_semantics) {
        if (existing_path[0] == '\0' || new_path[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno(ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&existing_path, &existing_path_name_nti) != 0)
            goto out;

        if (__translate_unix_to_amiga_path_name(&new_path, &new_path_name_nti) != 0)
            goto out;

        if (existing_path_name_nti.is_root || new_path_name_nti.is_root) {
            __set_errno(EACCES);
            goto out;
        }
    }

    D(("trying to get a lock on '%s'", existing_path));

    existing_path_lock = Lock((STRPTR) existing_path, SHARED_LOCK);
    if (existing_path_lock == BZERO) {
        SHOWMSG("that didn't work");

        __set_errno(__translate_access_io_error_to_errno(IoErr()));
        goto out;
    }

    D(("trying to make a link named '%s'", new_path));

    status = MakeLink((STRPTR) new_path, (APTR) existing_path_lock, LINK_HARD);
    if (status == DOSFALSE) {
        SHOWMSG("that didn't work");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    result = OK;

out:

    UnLock(existing_path_lock);

    RETURN(result);
    return (result);
}
