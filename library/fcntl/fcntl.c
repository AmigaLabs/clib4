/*
 * $Id: fcntl_fcntl.c,v 1.21 2023-07-04 14:39:23 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
fcntl(int file_descriptor, int cmd, ... /* int arg */) {
    struct file_action_message fam;
    struct flock *l;
    int vacant_slot;
    int result = ERROR;
    struct fd *fd = NULL;
    va_list arg;
    int error = 0;
    int flags;
    int fdbase;
    int i;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(file_descriptor);
    SHOWVALUE(cmd);

    if (__clib4->__fd[file_descriptor] == NULL || FLAG_IS_CLEAR(__clib4->__fd[file_descriptor]->fd_Flags, FDF_IN_USE) || file_descriptor < 0 || file_descriptor > __clib4->__num_fd) {
        __set_errno(EINVAL);
        goto out;
    }

    __set_errno(0);

    __check_abort_f(__clib4);

    /* F_DUPFD will need to modify the file descriptor table, which is why
       the stdio lock needs to be obtained here, before the individual
       file descriptor lock is held. */
    if (cmd == F_DUPFD)
        __stdio_lock(__clib4);

    fd = __get_file_descriptor(__clib4, file_descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        goto out;
    }

    __fd_lock(fd);

    switch (cmd) {
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:

            SHOWMSG("cmd=F_GETLK/F_SETLK/F_SETLKW");

            if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
                __set_errno(EINVAL);
                goto out;
            }

            if (fd->fd_File == BZERO) {
                __set_errno(EBADF);
                goto out;
            }

            va_start(arg, cmd);
            l = va_arg(arg, struct flock *);
            va_end(arg);

            assert(l != NULL);

            if (l == NULL) {
                __set_errno(EBADF);
                goto out;
            }

            if (l->l_type < F_RDLCK || l->l_type > F_WRLCK) {
                SHOWMSG("invalid flock type");

                __set_errno(EINVAL);
                break;
            }

            if (l->l_whence < SEEK_SET || l->l_whence > SEEK_END) {
                SHOWMSG("invalid flock offset");

                __set_errno(EINVAL);
                break;
            }

            if (__handle_record_locking(__clib4, cmd, l, fd, &error) < 0) {
                __set_errno(error);
                goto out;
            }

            result = OK;

            break;

        case F_GETFL:

            SHOWMSG("cmd=F_GETFL");

            if (FLAG_IS_SET(fd->fd_Flags, FDF_NON_BLOCKING))
                SET_FLAG(result, O_NONBLOCK);

            if (FLAG_IS_SET(fd->fd_Flags, FDF_ASYNC_IO))
                SET_FLAG(result, O_ASYNC);

            if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY))
                SET_FLAG(result, O_PATH);

            result = OK;

            break;

        case F_GETFD:
            /* Don't fail on this. If we reach this point file descriptor is ok */
            result = OK;

            break;

        case F_SETFD:

            /* We don't have any logic implemented here yet but don't fail if someone is asking to set a flag like FD_CLOEXEC */
            va_start(arg, cmd);
            flags = va_arg(arg, int);
            va_end(arg);

            /* Fail if flag is not FD_CLOEXEC, otherwise return OK */
            if (flags == FD_CLOEXEC)
                result = OK;

            break;

        case F_SETFL:

            SHOWMSG("cmd=F_SETFL");

            va_start(arg, cmd);
            flags = va_arg(arg, int);
            va_end(arg);

            /* If someone ask us o set STDIN_FILENO as O_NONBLOCK don't set it but don't return an error */
            if (file_descriptor == STDIN_FILENO) {
                if (FLAG_IS_SET(flags, O_NONBLOCK))
                    SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
                else
                    CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);

                result = OK;
                goto out;
            }

            /* If this is a file, make sure that we don't hit a zero file handle. */
            if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_SOCKET) && fd->fd_File == BZERO) {
                __set_errno(EBADF);
                goto out;
            }

            if ((FLAG_IS_SET(flags, O_NONBLOCK) && FLAG_IS_CLEAR(fd->fd_Flags, FDF_NON_BLOCKING)) ||
                (FLAG_IS_CLEAR(flags, O_NONBLOCK) && FLAG_IS_SET(fd->fd_Flags, FDF_NON_BLOCKING))) {
                fam.fam_Action = file_action_set_blocking;
                fam.fam_Arg = FLAG_IS_CLEAR(flags, O_NONBLOCK);

                assert(fd->fd_Action != NULL);

                if ((*fd->fd_Action)(__clib4, fd, &fam) < 0) {
                    __set_errno(fam.fam_Error);

                    goto out;
                }

                if (FLAG_IS_SET(flags, O_NONBLOCK))
                    SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
                else
                    CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
            }

            if ((FLAG_IS_SET(flags, O_ASYNC) && FLAG_IS_CLEAR(fd->fd_Flags, FDF_ASYNC_IO)) ||
                (FLAG_IS_CLEAR(flags, O_ASYNC) && FLAG_IS_SET(fd->fd_Flags, FDF_ASYNC_IO))) {
                fam.fam_Action = file_action_set_async;
                fam.fam_Arg = FLAG_IS_SET(flags, O_ASYNC);

                assert(fd->fd_Action != NULL);

                if ((*fd->fd_Action)(__clib4, fd, &fam) < 0) {
                    __set_errno(fam.fam_Error);

                    goto out;
                }

                if (FLAG_IS_SET(flags, O_ASYNC))
                    SET_FLAG(fd->fd_Flags, FDF_ASYNC_IO);
                else
                    CLEAR_FLAG(fd->fd_Flags, FDF_ASYNC_IO);
            }

            result = OK;

            break;

        case F_DUPFD:

            SHOWMSG("cmd=F_DUPFD");

            va_start(arg, cmd);
            fdbase = va_arg(arg, int);
            va_end(arg);

            if (fdbase < 0) {
                __set_errno(EINVAL);
                goto out;
            }

            /* Make sure that we have the required number of file descriptors available. */
            if (__grow_fd_table(__clib4, fdbase + 1) < 0)
                goto out;

            vacant_slot = -1;

            /* Guaranteed to have enough here */
            do {
                __stdio_unlock(__clib4);

                __check_abort_f(__clib4);

                __stdio_lock(__clib4);

                for (i = fdbase; i < __clib4->__num_fd; i++) {
                    if (FLAG_IS_CLEAR(__clib4->__fd[i]->fd_Flags, FDF_IN_USE)) {
                        vacant_slot = i;
                        break;
                    }
                }

                /* Didn't really find any, grow the table further */
                if (vacant_slot < 0 && __grow_fd_table(__clib4, 0) < 0)
                    goto out;
            } while (vacant_slot < 0);

            /* Got a file descriptor, duplicate it */
            __duplicate_fd(__clib4->__fd[vacant_slot], fd);

            result = vacant_slot;

            break;

        default:

            SHOWMSG("something else");

            __set_errno(ENOSYS);
            break;
    }

out:

    __fd_unlock(fd);

    if (cmd == F_DUPFD)
        __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
