/*
 * $Id: unistd_realpath.c,v 1.7 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char *
realpath(const char *path_name, char *buffer) {
    struct name_translation_info path_name_nti;
    struct name_translation_info buffer_nti;
    struct DevProc *dvp = NULL;
    BPTR lock = ZERO;
    char *result = NULL;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWSTRING(path_name);
    SHOWPOINTER(buffer);

    __check_abort();

    if (path_name == NULL || buffer == NULL) {
        SHOWSTRING("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    if (__clib2->__unix_path_semantics) {
        if (path_name[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno(ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;
    }

    D(("trying to get a lock on '%s'", path_name));

    lock = Lock((STRPTR) path_name, SHARED_LOCK);
    if (lock != ZERO) {
        LONG status;

        SHOWMSG("trying to obtain the absolute path");

        status = NameFromLock(lock, buffer, PATH_MAX);
        if (status == DOSFALSE) {
            SHOWMSG("that didn't work");

            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto out;
        }
    } else {
        SHOWMSG("couldn't get a lock");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    if (__clib2->__unix_path_semantics) {
        if (__translate_amiga_to_unix_path_name((char const **) &buffer, &buffer_nti) != 0)
            goto out;

        __restore_path_name((char const **) &buffer, &buffer_nti);

        strcpy(buffer, buffer_nti.substitute);
    }

    result = buffer;

    SHOWSTRING(buffer);

out:

    FreeDeviceProc(dvp);
    UnLock(lock);

    RETURN(result);
    return (result);
}
