/*
 * $Id: stat_mkdir.c,v 1.7 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

#include <dos/obsolete.h>

int mkdir(const char *path_name, mode_t mode) {
    struct name_translation_info path_name_nti;
    ULONG protection;
    int result = ERROR;
    BPTR dir_lock;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(mode);

    assert(path_name != NULL);

    __check_abort();

    if (path_name == NULL) {
        SHOWMSG("invalid path name parameter");

        __set_errno(EFAULT);
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno(EACCES);
            goto out;
        }
    }

    D(("trying to create '%s'", path_name));

    dir_lock = CreateDir((STRPTR) path_name);
    if (dir_lock == BZERO) {
        SHOWMSG("that didn't work");

        __set_errno(__translate_access_io_error_to_errno(IoErr()));
        goto out;
    }

    UnLock(dir_lock);

    protection = 0;

    if (FLAG_IS_SET(mode, S_IRUSR))
        SET_FLAG(protection, FIBF_READ);

    if (FLAG_IS_SET(mode, S_IWUSR)) {
        SET_FLAG(protection, FIBF_WRITE);
        SET_FLAG(protection, FIBF_DELETE);
    }

    if (FLAG_IS_SET(mode, S_IXUSR))
        SET_FLAG(protection, FIBF_EXECUTE);

    if (FLAG_IS_SET(mode, S_IRGRP))
        SET_FLAG(protection, FIBF_GRP_READ);

    if (FLAG_IS_SET(mode, S_IWGRP)) {
        SET_FLAG(protection, FIBF_GRP_WRITE);
        SET_FLAG(protection, FIBF_GRP_DELETE);
    }

    if (FLAG_IS_SET(mode, S_IXGRP))
        SET_FLAG(protection, FIBF_GRP_EXECUTE);

    if (FLAG_IS_SET(mode, S_IROTH))
        SET_FLAG(protection, FIBF_OTR_READ);

    if (FLAG_IS_SET(mode, S_IWOTH)) {
        SET_FLAG(protection, FIBF_OTR_WRITE);
        SET_FLAG(protection, FIBF_OTR_DELETE);
    }

    if (FLAG_IS_SET(mode, S_IXOTH))
        SET_FLAG(protection, FIBF_OTR_EXECUTE);

    SHOWSTRING(path_name);
    SHOWVALUE(protection);

    SetProtection((STRPTR) path_name, (LONG)(protection ^ (FIBF_READ | FIBF_WRITE | FIBF_EXECUTE | FIBF_DELETE)));

    result = OK;

out:

    RETURN(result);
    return (result);
}
