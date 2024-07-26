/*
 * $Id: stat_utimensat.c,v 2.0 2023-05-26 12:04:24 clib4devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

int
utimensat(int fd, const char *path, const struct timespec times[2], int flags) {
    ENTER();

    SHOWVALUE(fd);
    struct _clib4 *__clib4 = __CLIB4;
    int result = -1;
    struct DateStamp ds0;
    BOOL setToCurrentTime = FALSE;

    __check_abort_f(__clib4);

    if (!times || (times && times[0].tv_nsec == UTIME_NOW && times[1].tv_nsec == UTIME_NOW)) {
        setToCurrentTime = TRUE;
        DateStamp(&ds0);
        times = 0;
    }

    /*
     * EFAULT times pointed to an invalid address; or, dirfd was AT_FDCWD, and pathname is NULL or an invalid address.
    */
    if (fd == AT_FDCWD && path == NULL) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    /*
     * EINVAL pathname is NULL, dirfd is not AT_FDCWD, and flags contains AT_SYMLINK_NOFOLLOW.
     */
    if (path == NULL && fd != AT_FDCWD && FLAG_IS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    /*
     * ENAMETOOLONG (utimensat()) pathname is too long.
     */
    if (path != NULL && strlen(path) > PATH_MAX) {
        __set_errno_r(__clib4, ENAMETOOLONG);
        goto out;
    }

    /* If times is a valid timestamp convert it to a DateStamp */
    if (!setToCurrentTime) {
        struct timeval tv1;
        time_t mtime;
        // times[0] specifies the new "last access time" - Not used
        TIMESPEC_TO_TIMEVAL(&tv1, &times[1]);   // times[1] specifies the new "last modification time"

#ifdef DEBUG
        char tmbuf[64] = {0}, buf[64] = {0};
        time_t nowtime;
        struct tm *nowtm;
        nowtime = tv1.tv_sec;
        nowtm = localtime(&nowtime);
        strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
        snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv1.tv_usec);
        D(("%s\n", buf));
#endif

        if (CANNOT __convert_time_to_datestamp(tv1.tv_sec, &ds0)) {
            SHOWMSG("time conversion to datestamp failed");
            goto out;
        }
    }
    BOOL absolute = FALSE;
    struct name_translation_info path_name_nti;

    if (path != NULL) {
        /* Check for relative path */
        if (__clib4->__unix_path_semantics) {
            if (path[0] == '/')
                absolute = TRUE;

            if (__translate_unix_to_amiga_path_name(&path, &path_name_nti) != 0)
                goto out;
        }
        else {
            if (strchr(path, ':') != NULL)
                absolute = TRUE;
        }
        struct fd *fildes = __get_file_descriptor(__clib4, fd);
        /*
         * EBADF  (utimensat()) pathname is relative but dirfd is neither AT_FDCWD nor a valid file descriptor.
         */
        if (!absolute && (fd != AT_FDCWD && fildes == NULL)) {
            __set_errno_r(__clib4, EBADF);
            goto out;
        }
        /* If fd = AT_FDCWD but we get an absolute path return an error */
        else if (absolute && fd == AT_FDCWD) {
            __set_errno_r(__clib4, EINVAL);
            goto out;
        }
        /* Set date */
        if (SetDate(path, &ds0))
            result = 0;
    }
    else {
        struct ExamineData *dh;
        char *buffer = calloc(PATH_MAX + 1, 1);
        if (!buffer) {
            __set_errno_r(__clib4, ENOMEM);
            goto out;
        }

        struct fd *fildes = __get_file_descriptor(__clib4, fd);
        if (fildes == NULL) {
            __free_r(__clib4, buffer);
            __set_errno_r(__clib4, EBADF);
            goto out;
        }

        dh = ExamineObjectTags(EX_FileHandleInput, fildes->fd_File, TAG_DONE);
        if (dh == NULL) {
            __free_r(__clib4, buffer);
            __set_errno_r(__clib4, EBADF);
            goto out;
        }

        BOOL isLink = EXD_IS_LINK(dh);
        if (!isLink)
            strncpy(buffer, dh->Name, PATH_MAX);
        else
            strncpy(buffer, dh->Link, PATH_MAX);
        FreeDosObject(DOS_EXAMINEDATA, dh);

        SHOWSTRING(buffer);

        /* Don't know if this work actually on OS4
         * Changing symlink time or regular file will always change both files
         */
        if (isLink && FLAG_IS_SET(flags, AT_SYMLINK_FOLLOW)) {
            struct DevProc *dvp = NULL;
            dvp = GetDeviceProc(buffer, dvp);
            if (dvp == NULL) {
                free(buffer);
                goto out;
            }
            char new_name[PATH_MAX + 1] = {0};
            LONG slResult = ReadSoftLink(dvp->dvp_Port, dvp->dvp_Lock, buffer, (STRPTR) &new_name, (LONG) PATH_MAX);
            FreeDeviceProc(dvp);
            if (result < 0) {
                free(buffer);
                goto out;
            }
            /* Set date */
            if (SetDate((CONST_STRPTR) &new_name, &ds0))
                result = 0;
        }
        else {
            /* Set date */
            if (SetDate(buffer, &ds0))
                result = 0;
        }
        __free_r(__clib4, buffer);
    }

out:
    RETURN(result);
    return (result);
}