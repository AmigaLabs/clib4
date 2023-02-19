/*
 * $Id: unistd_access.c,v 1.10 2021-03-07 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
access(const char *path_name, int mode) {
    struct name_translation_info path_name_nti;
    int result = ERROR;
    BPTR lock = ZERO;
    struct ExamineData *status = NULL;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(mode);

    assert(path_name != NULL);

    __check_abort();

    if (path_name == NULL) {
        SHOWMSG("invalid path name");

        __set_errno(EFAULT);
        goto out;
    }

    if (mode < 0 || mode > (R_OK | W_OK | X_OK | F_OK)) {
        SHOWMSG("invalid mode");

        __set_errno(EINVAL);
        goto out;
    }

    STRPTR actual_path_name = NULL;

    if (__unix_path_semantics) {
        {
            if (path_name[0] == '\0') {
                SHOWMSG("no name given");

                __set_errno(ENOENT);
                goto out;
            }

            if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
                goto out;

            if (NOT path_name_nti.is_root)
            actual_path_name = (STRPTR) path_name;
        }

        if (actual_path_name != NULL) {
            D(("trying to get a lock on '%s'", actual_path_name));

            lock = Lock(actual_path_name, SHARED_LOCK);
            if (lock == ZERO) {
                __set_errno(__translate_access_io_error_to_errno(IoErr()));
                goto out;
            }
        }
    } else {
        D(("trying to get a lock on '%s'", path_name));

        lock = Lock((STRPTR) path_name, SHARED_LOCK);
        if (lock == ZERO) {
            __set_errno(__translate_access_io_error_to_errno(IoErr()));
            goto out;
        }
    }

    if ((mode != F_OK) && (mode & (R_OK | W_OK | X_OK)) != 0) {
        if (__unix_path_semantics) {
            if (lock == ZERO) {
                memset(status, 0, sizeof(*status));

                /* This is a simulated directory which cannot be
                 * modified under program control.
                 * TODO - CHECK THIS UNDER OS4 AND EXAMINEOBJECT!
                 */
                status->Protection = EXDF_NO_WRITE;
                status->Type = FSO_TYPE_DIRECTORY;
            } else {
                status = ExamineObjectTags(EX_LockInput, lock, TAG_DONE);
                if (status == NULL) {
                    SHOWMSG("couldn't examine");

                    __set_errno(__translate_io_error_to_errno(IoErr()));
                    goto out;
                }
            }
        } else {
            status = ExamineObjectTags(EX_LockInput, lock, TAG_DONE);
            if (status == DOSFALSE) {
                SHOWMSG("couldn't examine");

                __set_errno(__translate_io_error_to_errno(IoErr()));
                goto out;
            }
        }

        if (FLAG_IS_SET(mode, R_OK)) {
            if (FLAG_IS_SET(status->Protection, EXDF_NO_READ)) {
                SHOWMSG("not readable");

                __set_errno(EACCES);
                goto out;
            }
        }

        if (FLAG_IS_SET(mode, W_OK)) {
            if (FLAG_IS_SET(status->Protection, EXDF_NO_WRITE) ||
                FLAG_IS_SET(status->Protection, EXDF_NO_DELETE)) {
                SHOWMSG("not writable");

                __set_errno(EACCES);
                goto out;
            }
        }

        if (FLAG_IS_SET(mode, X_OK)) {
            /* Note: 'X' means 'search' for directories, which is always
             *       permitted on the Amiga.
             */
            if ((!EXD_IS_DIRECTORY(status)) && FLAG_IS_SET(status->Protection, EXDF_NO_EXECUTE)) {
                SHOWMSG("not executable");

                __set_errno(EACCES);
                goto out;
            }
        }
    }

    result = OK;

    out:

    if (NULL != status) {
        FreeDosObject(DOS_EXAMINEDATA, status);
        status = NULL;
    }

    UnLock(lock);

    RETURN(result);
    return (result);
}
