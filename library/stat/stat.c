/*
 * $Id: stat_stat.c,v 1.11 2021-01-31 12:04:24 clib4devs Exp $
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
stat(const char *path_name, struct stat *st) {
    struct name_translation_info path_name_nti;
    struct ExamineData *fib = NULL;
    struct Lock *fl;
    int result = ERROR;
    BPTR file_lock = BZERO;
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
            * We make up some pseudo data for it.
            */
        if (path_name_nti.is_root) {
            struct DateStamp ds;
            time_t mtime;

            SHOWMSG("seting up the root directory info");

            memset(st, 0, sizeof(*st));

            DateStamp(&ds);

            mtime = __convert_datestamp_to_time(&ds);

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

    file_lock = Lock((STRPTR) path_name, SHARED_LOCK);
    if (file_lock == BZERO) {
        SHOWMSG("that didn't work");

        __set_errno_r(__clib4, __translate_access_io_error_to_errno(IoErr()));
        goto out;
    }

    fib = ExamineObjectTags(EX_LockInput, file_lock, TAG_DONE);
    if (fib == NULL) {
        SHOWMSG("couldn't examine it");

        __set_errno_r(__clib4, __translate_io_error_to_errno(IoErr()));
        goto out;
    }

    fl = BADDR(file_lock);

    __convert_file_info_to_stat(fl->fl_Port, fib, st);

    result = OK;

out:

    FreeDosObject(DOS_EXAMINEDATA, fib);
    UnLock(file_lock);

    RETURN(result);
    return (result);
}
