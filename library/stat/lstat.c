/*
 * $Id: stat_lstat.c,v 1.16 2021-01-31 09:25:28 clib4devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

int
lstat(const char *path_name, struct stat *st) {
    struct name_translation_info path_name_nti;
    struct Lock *fl;
    int result = ERROR;
    struct ExamineData *fib = NULL;
    BPTR file_lock = BZERO;
    int link_length = -1;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path_name);
    SHOWPOINTER(st);

    assert(path_name != NULL && st != NULL);

    __check_abort_f(__clib4);

    if (path_name == NULL || st == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        if (path_name[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno_r(__clib4, ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        /* The pseudo root directory is a very special case indeed.
            We make up some pseudo data for it. */
        if (path_name_nti.is_root) {
            time_t mtime = 0;

            SHOWMSG("setting up the root directory info");

            memset(st, 0, sizeof(*st));

            time(&mtime);

            st->st_mode = S_IFDIR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
            st->st_mtime = mtime;
            st->st_atime = mtime;
            st->st_ctime = mtime;
            st->st_nlink = 2;
            st->st_blksize = 512;

            result = OK;

            goto out;
        }
    }

    D(("trying to get a lock on '%s'", path_name));

    file_lock = __lock(path_name, SHARED_LOCK, &link_length, NULL, 0);
    if (file_lock == BZERO && link_length < 0) {
        SHOWMSG("that didn't work");

        __set_errno_r(__clib4, __translate_access_io_error_to_errno(IoErr()));
        goto out;
    }

    if (link_length > 0) {
        time_t mtime = 0;
        struct DevProc *dvp = GetDeviceProcFlags((STRPTR) path_name, 0, LDF_ALL);
        struct MsgPort *port = NULL;
        if (dvp) {
            if (!(dvp->dvp_Flags & DVPF_UNLOCK)) {
                SetIoErr(dvp->dvp_Lock);
                port = dvp->dvp_Port;
            }
            FreeDeviceProc(dvp);
        }
        /* Build a dummy stat for the link. */

        SHOWMSG("Creating stat info for link.");

        memset(st, 0, sizeof(*st));

        time(&mtime);

        st->st_mode = S_IFLNK | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
        st->st_dev = (dev_t) port;
        st->st_size = link_length;
        st->st_mtime = mtime;
        st->st_atime = mtime;
        st->st_ctime = mtime;
        st->st_nlink = 1;
    } else {
        fib = ExamineObjectTags(EX_LockInput, file_lock, TAG_DONE);
        if (fib == NULL) {
            SHOWMSG("couldn't examine it");

            __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
            goto out;
        }

        fl = BADDR(file_lock);

        __convert_file_info_to_stat(fl->fl_Port, fib, st);
    }

    result = OK;

out:

    FreeDosObject(DOS_EXAMINEDATA, fib);
    UnLock(file_lock);

    RETURN(result);
    return (result);
}
