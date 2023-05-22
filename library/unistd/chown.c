/*
 * $Id: unistd_chown.c,v 1.12 2006-09-25 15:41:50 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int chown(const char *path_name, uid_t owner, gid_t group) {
    struct name_translation_info path_name_nti;
    struct DevProc *dvp = NULL;
    BPTR file_lock = BZERO;
    BOOL owner_changed = TRUE;
    struct ExamineData *status = NULL;
    int result = ERROR;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(owner);
    SHOWVALUE(group);

    assert(path_name != NULL);

    __check_abort();

    if (path_name == NULL) {
        SHOWMSG("invalid path name");

        __set_errno(EFAULT);
        goto out;
    }

    if (__clib2->__unix_path_semantics) {
        if (path_name[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno(ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno(EACCES);
            goto out;
        }
    }

    /* A value of -1 for either the owner or the group ID means
       that what's currently being used should not be changed. */
    if (owner == (uid_t) - 1 || group == (gid_t) - 1) {
        D_S(struct FileInfoBlock, fib);

        /* Try to find out which owner/group information
           is currently in use. */
        file_lock = Lock((STRPTR) path_name, SHARED_LOCK);
        if (file_lock == BZERO || ExamineObjectTags(EX_LockInput, file_lock, TAG_DONE) == NULL) {
            __set_errno(__translate_access_io_error_to_errno(IoErr()));
            goto out;
        }

        UnLock(file_lock);
        file_lock = BZERO;

        /* Replace the information that should not be changed. */
        if (owner == (uid_t) - 1)
            owner = fib->fib_OwnerUID;

        if (group == (gid_t) - 1)
            group = fib->fib_OwnerGID;

        /* Is anything different at all? */
        if (owner == fib->fib_OwnerUID && group == fib->fib_OwnerGID)
            owner_changed = FALSE;
    }

    if (owner > 65535 || group > 65535) {
        SHOWMSG("invalid owner or group");

        __set_errno(EINVAL);
        goto out;
    }

    if (owner_changed) {
        D(("changing owner of '%s'", path_name));

        int32 ret = SetOwnerInfoTags(OI_StringNameInput, (STRPTR) path_name, OI_OwnerUID,
                                     (LONG)((((ULONG) owner) << 16) | group), TAG_DONE);
        if (ret == DOSFALSE) {
            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto out;
        }
    }

    result = OK;

out:
    if (status != NULL) {
        FreeDosObject(DOS_EXAMINEDATA, status);
    }

    FreeDeviceProc(dvp);

    if (file_lock != BZERO)
        UnLock(file_lock);

    RETURN(result);
    return (result);
}
