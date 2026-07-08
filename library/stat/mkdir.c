/*
 * $Id: stat_mkdir.c,v 1.8 2024-03-25 12:04:24 clib4devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

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

    __check_abort_f(__clib4);

    if (path_name == NULL) {
        SHOWMSG("invalid path name parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno_r(__clib4, EACCES);
            goto out;
        }
    }

    char path_to_create[PATH_MAX] = {0};
    strncpy(path_to_create, path_name, PATH_MAX - 1);

    /* This check avoid that for some mistakes a directory contain more than one : char inside the name
     * This could help to avoid problems on SFS file system that allow (by mistake) the creation of dirs
     * like DriveName:Dir/DriveName:Dir
     */

    int counter = 0;
    size_t len = strlen(path_to_create);
    for (size_t i = 0; i < len; i++) {
        if (path_to_create[i] == ':') {
            counter++;
        }
    }
    if (counter > 1) {
        SHOWMSG("Invalid directory name parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    if (len > 0 && path_to_create[len - 1] == '/') {
        path_to_create[len - 1] = '\0'; // Remove '/' if present
    }

    D(("trying to create '%s'", path_to_create));

    dir_lock = CreateDir((STRPTR) path_to_create);
    if (dir_lock == BZERO) {
        SHOWMSG("that didn't work");

        __set_errno_r(__clib4, __translate_access_io_error_to_errno(IoErr()));
        goto out;
    }

    UnLock(dir_lock);

    protection = 0;

    if (FLAG_IS_SET(mode, S_IRUSR))
        SET_FLAG(protection, EXDF_NO_READ);

    if (FLAG_IS_SET(mode, S_IWUSR)) {
        SET_FLAG(protection, EXDF_NO_WRITE);
        SET_FLAG(protection, EXDF_NO_DELETE);
    }

    if (FLAG_IS_SET(mode, S_IXUSR))
        SET_FLAG(protection, EXDF_NO_EXECUTE);

    if (FLAG_IS_SET(mode, S_IRGRP))
        SET_FLAG(protection, EXDF_GRP_READ);

    if (FLAG_IS_SET(mode, S_IWGRP)) {
        SET_FLAG(protection, EXDF_GRP_WRITE);
        SET_FLAG(protection, EXDF_GRP_DELETE);
    }

    if (FLAG_IS_SET(mode, S_IXGRP))
        SET_FLAG(protection, EXDF_GRP_EXECUTE);

    if (FLAG_IS_SET(mode, S_IROTH))
        SET_FLAG(protection, EXDF_OTR_READ);

    if (FLAG_IS_SET(mode, S_IWOTH)) {
        SET_FLAG(protection, EXDF_OTR_WRITE);
        SET_FLAG(protection, EXDF_OTR_DELETE);
    }

    if (FLAG_IS_SET(mode, S_IXOTH))
        SET_FLAG(protection, EXDF_OTR_EXECUTE);

    SHOWSTRING(path_to_create);
    SHOWVALUE(protection);

    SetProtection((STRPTR) path_to_create, (LONG)(protection ^ (EXDF_NO_READ | EXDF_NO_WRITE | EXDF_NO_EXECUTE | EXDF_NO_DELETE)));

    result = OK;

out:

    RETURN(result);
    return (result);
}
