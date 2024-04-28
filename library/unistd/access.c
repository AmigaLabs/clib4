/*
 * $Id: unistd_access.c,v 1.10 2021-03-07 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
access(const char *path_name, int mode) {
    struct name_translation_info path_name_nti;
    int result = ERROR;
    BPTR lock = BZERO;
    struct ExamineData *status = NULL;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(mode);

    assert(path_name != NULL);

    __check_abort_f(__clib4);

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

    if (__clib4->__unix_path_semantics) {
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
            if (lock == BZERO) {
                __set_errno(__translate_access_io_error_to_errno(IoErr()));
                goto out;
            }
        }
    } else {
        D(("trying to get a lock on '%s'", path_name));

        lock = Lock((STRPTR) path_name, SHARED_LOCK);
        if (lock == BZERO) {
            __set_errno(__translate_access_io_error_to_errno(IoErr()));
            goto out;
        }
    }

    if ((mode != F_OK) && (mode & (R_OK | W_OK | X_OK)) != 0) {
        if (__clib4->__unix_path_semantics) {
            if (lock == BZERO) {
                status = AllocDosObjectTags(DOS_EXAMINEDATA, TAG_END);
                if(!status) {
                    __set_errno(ENOMEM);
                    goto out;
                }

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

    FreeDosObject(DOS_EXAMINEDATA, status);
    UnLock(lock);

    RETURN(result);
    return (result);
}
