/*
 * $Id: utime_utime.c,v 1.12 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UTIME_HEADERS_H
#include "utime_headers.h"
#endif /* _UTIME_HEADERS_H */

int
utime(const char *path_name, const struct utimbuf *times) {
    struct name_translation_info path_name_nti;
    struct DateStamp ds;
    int result = ERROR;
    LONG status;
    struct _clib4 *__clib4 = __CLIB4;

    assert(path_name != NULL);

    __check_abort_f(__clib4);

    if (path_name == NULL) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    /* If a modification time is provided, convert it into the local
       DateStamp format, as used by the SetDate() function. */
    if (times != NULL) {
        if (CANNOT __convert_time_to_datestamp(times->modtime, &ds)) {
            __set_errno_r(__clib4, EINVAL);
            goto out;
        }
    } else {
        /* No special modification time provided; use the current
           time instead. */
        DateStamp(&ds);
    }

    if (__clib4->__unix_path_semantics) {
        if (path_name[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno_r(__clib4, ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno_r(__clib4, EACCES);
            goto out;
        }
    }

    status = SetDate((STRPTR) path_name, &ds);
    if (status == DOSFALSE) {
        __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        goto out;
    }

    result = OK;

out:

    return (result);
}
