/*
 * $Id: mount_statfs.c,v 1.7 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _MOUNT_HEADERS_H
#include "mount_headers.h"
#endif /* _MOUNT_HEADERS_H */

int statfs(const char *path, struct statfs *buf) {
    struct name_translation_info path_nti;
    D_S(struct InfoData, id);
    LONG status;
    BPTR lock = BZERO;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path);
    SHOWPOINTER(buf);

    __check_abort_f(__clib4);

    assert(path != NULL && buf != NULL);

    if (path == NULL || buf == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        if (path[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno_r(__clib4, ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path, &path_nti) != 0)
            goto out;

        SHOWSTRING(path);

        /* The pseudo root directory is a very special case indeed.
            * We make up some pseudo data for it.
            */
        if (path_nti.is_root) {
            SHOWMSG("returning data for / directory");

            memset(buf, 0, sizeof(*buf));

            buf->f_bsize = 512;
            buf->f_blocks = 1;
            buf->f_bfree = 1;
            buf->f_iosize = buf->f_bsize;
            buf->f_bavail = buf->f_bfree;
            buf->f_flags = MNT_NOATIME | MNT_SYMPERM | MNT_LOCAL | MNT_RDONLY;

            result = OK;

            goto out;
        }
    }

    D(("trying to get a lock on '%s'", path));

    lock = Lock((STRPTR) path, SHARED_LOCK);
    if (lock == BZERO) {
        SHOWMSG("that didn't work");

        __set_errno_r(__clib4, __translate_access_io_error_to_errno(IoErr()));
        goto out;
    }

    status = Info(lock, id);
    if (status == DOSFALSE) {
        SHOWMSG("didn't get the info");

        __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        goto out;
    }

    __convert_info_to_statfs(id, buf);

    result = OK;

out:

    UnLock(lock);

    RETURN(result);
    return (result);
}
