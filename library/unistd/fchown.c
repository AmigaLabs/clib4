/*
 * $Id: unistd_fchown.c,v 1.16 2021-01-31 14:39:23 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
fchown(int file_descriptor, uid_t owner, gid_t group) {
    struct ExamineData *fib;
    BPTR parent_dir = BZERO;
    BPTR old_current_dir = BZERO;
    BOOL current_dir_changed = FALSE;
    int result = ERROR;
    struct fd *fd = NULL;
    LONG success;
    struct _clib4 *__clib4 = __CLIB4;
    BOOL isFdLocked = FALSE;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWVALUE(owner);
    SHOWVALUE(group);

    assert(file_descriptor >= 0 && file_descriptor < __clib4->__num_fd);
    assert(__clib4->__fd[file_descriptor] != NULL);
    assert(FLAG_IS_SET(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);
    isFdLocked = TRUE;

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
        __set_errno(EINVAL);
        goto out;
    }

    if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO)) {
        __set_errno(EBADF);
        goto out;
    }

    fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
    success = (fib != NULL && (parent_dir = ParentOfFH(fd->fd_File)) != BZERO);
    if (NO success) {
        SHOWMSG("couldn't find parent directory");

        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    old_current_dir = SetCurrentDir(parent_dir);
    current_dir_changed = TRUE;

    /* A value of -1 for either the owner or the group ID means
       that what's currently being used should not be changed. */
    if (owner == (uid_t) - 1)
        owner = fib->OwnerUID;

    if (group == (gid_t) - 1)
        group = fib->OwnerGID;

    /* Check if the owner and group IDs are usable. This test
       follows the comparison against -1 above just so that we
       can be sure that we are not mistaking a -1 for a
       large unsigned number. */
    if (owner > 65535 || group > 65535) {
        SHOWMSG("owner or group not OK");

        SHOWVALUE(owner);
        SHOWVALUE(group);

        __set_errno(EINVAL);
        goto out;
    }

    /* Did anything change at all? */
    if (group != fib->OwnerUID || owner != fib->OwnerUID) {
        success = SetOwnerInfoTags(OI_StringNameInput, fib->Name, OI_OwnerUID,
                                   (LONG)((((ULONG) owner) << 16) | (ULONG) group), TAG_DONE);
        if (NO success) {
            SHOWMSG("couldn't change owner/group");

            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto out;
        }
    }

    result = OK;

out:

    if (isFdLocked)
        __fd_unlock(fd);

    UnLock(parent_dir);

    if (current_dir_changed)
        SetCurrentDir(old_current_dir);

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
