/*
 * $Id: unistd_pathconf.c,v 1.1 2006-07-28 14:37:28 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

long
pathconf(const char *path, int name) {
    struct name_translation_info path_name_nti;
    struct DevProc *dvp = NULL;
    BOOL ignore_port = FALSE;
    long ret = -1;

    ENTER();

    SHOWSTRING(path);
    SHOWVALUE(name);

    if (path == NULL) {
        SHOWMSG("invalid path name");

        __set_errno(EFAULT);
        goto out;
    }

    if (__CLIB2->__unix_path_semantics) {
        if (path[0] == '\0') {
            SHOWMSG("Empty name");

            __set_errno(ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            /* Should we disallow / or use OFS as the lowest common denominator? */
            ignore_port = TRUE;
        }
    }

    if (!ignore_port) {
        dvp = GetDeviceProc((STRPTR) path, NULL);
        if (dvp == NULL) {
            __set_errno(__translate_access_io_error_to_errno(IoErr()));
            goto out;
        }
    }

    ret = __pathconf((dvp != NULL) ? dvp->dvp_Port : NULL, name);

out:

    if (dvp != NULL) {
        FreeDeviceProc(dvp);
        dvp = NULL;
    }

    RETURN(ret);
    return (ret);
}
