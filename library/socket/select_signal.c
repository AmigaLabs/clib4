/*
 * $Id: socket_select_signal.c,v 1.8 2024-07-22 14:39:23 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

STATIC void
copy_fd_set(fd_set *to, fd_set *from, int num_fds) {
    ENTER();

    SHOWPOINTER(to);
    SHOWPOINTER(from);
    SHOWVALUE(num_fds);

    if (to != NULL && from != NULL && num_fds > 0) {
        size_t num_bytes;

        num_bytes = sizeof(unsigned long) * ((num_fds + 31) / 32);
        SHOWVALUE(num_bytes);
        memmove(to, from, num_bytes);
    }

    LEAVE();
}

STATIC void
zero_fd_set(fd_set *set, int num_fds) {
    ENTER();

    SHOWPOINTER(set);
    SHOWVALUE(num_fds);

    if (set != NULL && num_fds > 0) {
        size_t num_bytes;

        num_bytes = sizeof(unsigned long) * ((num_fds + 31) / 32);

        SHOWVALUE(num_bytes);

        memset(set, 0, num_bytes);
    }

    LEAVE();
}

static fd_set *
allocate_fd_set(struct _clib4 *__clib4, int num_fds, fd_set *duplicate_this_set) {
    fd_set *result = NULL;
    size_t num_bytes;
    fd_set *set;

    ENTER();

    assert(num_fds > 0);

    SHOWVALUE(num_fds);

    if (num_fds <= 0) {
        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    num_bytes = sizeof(unsigned long) * ((num_fds + 31) / 32);

    SHOWVALUE(num_bytes);

    set = (fd_set *) __malloc_r(__clib4, num_bytes);
    if (set != NULL) {
        if (duplicate_this_set != NULL)
            copy_fd_set(set, duplicate_this_set, num_fds);
        else
            zero_fd_set(set, num_fds);

        result = set;
    }

out:
    RETURN(result);
    return (result);
}

static void
free_fd_set(struct _clib4 *__clib4, fd_set *set) {
    if (set != NULL)
        __free_r(__clib4, set);
}

static struct fd *
get_file_descriptor(struct _clib4 *__clib4, int file_descriptor) {
    struct fd *result = NULL;
    struct fd *fd;

    __stdio_lock(__clib4);

    if (file_descriptor < 0 || file_descriptor >= __clib4->__num_fd)
        goto out;

    assert(__clib4->__fd != NULL);

    fd = __clib4->__fd[file_descriptor];

    assert(fd != NULL);

    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE))
        goto out;

    result = fd;

out:

    __stdio_unlock(__clib4);

    return (result);
}

static void
map_descriptor_sets(
        struct _clib4 *__clib4,
        const fd_set *input_fds,
        int num_input_fds,

        fd_set *socket_fds,
        int num_socket_fds,
        int *total_socket_fd_ptr,

        fd_set *file_fds,
        int num_file_fds,
        int *total_file_fd_ptr) {
    ENTER();

    SHOWPOINTER(input_fds);
    SHOWVALUE(num_input_fds);

    SHOWPOINTER(socket_fds);
    SHOWVALUE(num_socket_fds);

    SHOWPOINTER(file_fds);
    SHOWVALUE(num_file_fds);

    /* This routine maps file descriptor sets
     * from one format to another. We map
     * socket descriptors and regular file
     * descriptor sets.
     */
    if (input_fds != NULL && num_input_fds > 0) {
        int total_socket_fd;
        int total_file_fd;
        struct fd *fd;
        int file_fd;

        total_socket_fd = (*total_socket_fd_ptr);
        total_file_fd = (*total_file_fd_ptr);

        SHOWVALUE(total_socket_fd);
        SHOWVALUE(total_file_fd);

        for (file_fd = 0; file_fd < num_input_fds; file_fd++) {
            if (NOT FD_ISSET(file_fd, input_fds))
                continue;

            D(("descriptor %ld is set", file_fd));

            fd = get_file_descriptor(__clib4, file_fd);
            if (fd == NULL) {
                SHOWMSG("but no file is attached to it");
                continue;
            }

            /* Is this a socket descriptor? */
            if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
                int socket_fd = fd->fd_Socket;

                D(("corresponds to socket #%ld", socket_fd));
                if (socket_fds != NULL && socket_fd < num_socket_fds) {
                    SHOWMSG("setting it");
                    FD_SET(socket_fd, socket_fds);

                    if (total_socket_fd < socket_fd + 1)
                        total_socket_fd = socket_fd + 1;
                } else {
                    SHOWMSG("can't set it, though");
                }
            } else {
                /* We watch files bound to console streams and disk files which may have data stored in them. */
                if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO) && FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_INTERACTIVE)) {
                    SHOWMSG("this is a file, or otherwise unsuitable");
                    continue;
                }

                /* If we are dealing with STDIN, mark it as Socket. This flag is used in
                 * read() calls to handle carriage return
                 */
                if (fd->fd_File == STDIN_FILENO) {
                    SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
                    SET_FLAG(fd->fd_Flags, FDF_STDIN_AS_SOCKET);
                }

                if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE) || FLAG_IS_SET(fd->fd_Flags, FDF_POLL)) {
                    SHOWMSG("this is an interactive / poll stream");
                } else {
                    struct ExamineData *fib;

                    /* Let's see if we can examine the file. Some file systems
                       may not support this. */
                    fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
                    if (fib == NULL) {
                        SHOWMSG("file is unusable; we cannot examine the file.");
                        continue;
                    }

                    /* If we can't make assumptions about the file position, then
                       this better be a pipe. */
                    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_CACHE_POSITION) && fib->Type != ST_PIPEFILE) {
                        FreeDosObject(DOS_EXAMINEDATA, fib);

                        SHOWMSG("file is unusable; it is not a file system and not a pipe.");
                        continue;
                    }
                    FreeDosObject(DOS_EXAMINEDATA, fib);
                }

                if (file_fds != NULL && file_fd < num_file_fds) {
                    SHOWMSG("setting it");

                    FD_SET(file_fd, file_fds);

                    if (total_file_fd < file_fd + 1)
                        total_file_fd = file_fd + 1;
                } else {
                    SHOWMSG("can't set it, though");
                }
            }
        }

        (*total_socket_fd_ptr) = total_socket_fd;
        (*total_file_fd_ptr) = total_file_fd;

        SHOWVALUE(total_socket_fd);
        SHOWVALUE(total_file_fd);
    }

    LEAVE();
}

static void
remap_descriptor_sets(
        struct _clib4 *__clib4,
        const fd_set *socket_fds,
        int num_socket_fds,

        const fd_set *file_fds,
        int num_file_fds,

        fd_set *output_fds,
        int num_output_fds) {
    ENTER();

    SHOWPOINTER(socket_fds);
    SHOWVALUE(num_socket_fds);

    SHOWPOINTER(file_fds);
    SHOWVALUE(num_file_fds);

    SHOWPOINTER(output_fds);
    SHOWVALUE(num_output_fds);

    /* This routine reverses the mapping established
     * above. We map the file and socket descriptor
     * sets back into the original set.
     */
    if (output_fds != NULL && num_output_fds > 0) {
        zero_fd_set(output_fds, num_output_fds);

        if (socket_fds != NULL && num_socket_fds > 0) {
            struct fd *fd;
            int output_fd;
            int socket_fd;

            SHOWMSG("taking care of the sockets");

            for (socket_fd = 0; socket_fd < num_socket_fds; socket_fd++) {
                if (NOT FD_ISSET(socket_fd, socket_fds)) {
                    continue;
                }

                for (output_fd = 0; output_fd < num_output_fds; output_fd++) {
                    fd = get_file_descriptor(__clib4, output_fd);
                    if (fd != NULL && FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET) && fd->fd_Socket == socket_fd) {
                        assert(output_fd < num_output_fds);
                        assert(FLAG_IS_SET(__clib4->__fd[output_fd]->fd_Flags, FDF_IS_SOCKET));

                        D(("setting file %ld for socket #%ld", output_fd, socket_fd));

                        FD_SET(output_fd, output_fds);
                    }
                }
            }
        }

        if (file_fds != NULL && num_file_fds > 0) {
            int file_fd;

            SHOWMSG("taking care of the files");

            for (file_fd = 0; file_fd < num_file_fds; file_fd++) {
                if (FD_ISSET(file_fd, file_fds)) {
                    int output_fd = file_fd;

                    assert(output_fd < num_output_fds);
                    assert(FLAG_IS_CLEAR(__clib4->__fd[output_fd]->fd_Flags, FDF_IS_SOCKET));

                    D(("setting file %ld", file_fd));

                    FD_SET(output_fd, output_fds);
                }
            }
        }
    } else {
        SHOWMSG("no output necessary");
    }

    LEAVE();
}

static void
get_num_descriptors_used(struct _clib4 *__clib4, int num_fds, int *num_socket_used_ptr, int *num_file_used_ptr) {
    int num_socket_used = 0;
    int num_file_used = 0;
    int which_file_fd;
    struct fd *fd;

    assert(num_socket_used_ptr != NULL);
    assert(num_file_used_ptr != NULL);

    SHOWMSG("figuring out which file descriptors are in use");

    for (which_file_fd = 0; which_file_fd < num_fds; which_file_fd++) {
        fd = get_file_descriptor(__clib4, which_file_fd);
        if (fd != NULL) {
            if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET)) {
                int which_socket_fd = fd->fd_Socket;

                if (num_socket_used < which_socket_fd + 1)
                    num_socket_used = which_socket_fd + 1;
            } else {
                if (num_file_used < which_file_fd + 1)
                    num_file_used = which_file_fd + 1;
            }
        }
    }

    (*num_socket_used_ptr) = num_socket_used;
    (*num_file_used_ptr) = num_file_used;
}

int
__select(int num_fds, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds, struct timeval *timeout, ULONG *signal_mask_ptr) {
    fd_set *backup_socket_read_fds = NULL;
    fd_set *backup_socket_write_fds = NULL;
    fd_set *backup_socket_except_fds = NULL;

    fd_set *backup_file_read_fds = NULL;
    fd_set *backup_file_write_fds = NULL;

    fd_set *socket_read_fds = NULL;
    fd_set *socket_write_fds = NULL;
    fd_set *socket_except_fds = NULL;
    int total_socket_fd;

    fd_set *file_read_fds = NULL;
    fd_set *file_write_fds = NULL;
    int total_file_fd;

    ULONG signal_mask;

    struct fd *fd;
    int result = ERROR;

    int num_socket_used;
    int num_file_used;
    int i;

    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(num_fds);
    SHOWPOINTER(read_fds);
    SHOWPOINTER(write_fds);
    SHOWPOINTER(except_fds);
    SHOWPOINTER(timeout);

    if (timeout != NULL) {
        SHOWVALUE(timeout->tv_sec);
        SHOWVALUE(timeout->tv_usec);
    }

    DECLARE_SOCKETBASE();

    if (signal_mask_ptr != NULL) {
        signal_mask = (*signal_mask_ptr);
        (*signal_mask_ptr) = 0;
    } else {
        signal_mask = 0;
    }

    __check_abort_f(__clib4);

    /* Figure out the number of file and socket descriptors in use. */
    get_num_descriptors_used(__clib4, num_fds, &num_socket_used, &num_file_used);

    SHOWVALUE(num_socket_used);
    SHOWVALUE(num_file_used);

    /* Dynamically allocate the tables to keep track of which descriptor
     * is ready for I/O.
     */
    if (read_fds != NULL) {
        if (num_socket_used > 0) {
            SHOWMSG("allocating read socket fd_set");

            socket_read_fds = allocate_fd_set(__clib4, num_socket_used, NULL);
            if (socket_read_fds == NULL)
                goto out;
        }

        if (num_file_used > 0) {
            SHOWMSG("allocating read file fd_set");

            file_read_fds = allocate_fd_set(__clib4, num_file_used, NULL);
            if (file_read_fds == NULL)
                goto out;
        }
    }

    if (write_fds != NULL) {
        if (num_socket_used > 0) {
            SHOWMSG("allocating write socket fd_set");

            socket_write_fds = allocate_fd_set(__clib4, num_socket_used, NULL);
            if (socket_write_fds == NULL)
                goto out;
        }

        if (num_file_used > 0) {
            SHOWMSG("allocating write file fd_set");

            file_write_fds = allocate_fd_set(__clib4, num_file_used, NULL);
            if (file_write_fds == NULL)
                goto out;
        }
    }

    if (except_fds != NULL) {
        if (num_socket_used > 0) {
            SHOWMSG("allocating except socket fd_set");

            socket_except_fds = allocate_fd_set(__clib4, num_socket_used, NULL);
            if (socket_except_fds == NULL)
                goto out;
        }
    }

    total_socket_fd = 0;
    total_file_fd = 0;

    SHOWMSG("mapping the fd_sets");

    /* Translate from the tables the caller provided to us to the local copies,
     * which take the files and sockets into account.
     */
    __stdio_lock(__clib4);

    map_descriptor_sets(__clib4, read_fds,   num_fds, socket_read_fds,   num_socket_used, &total_socket_fd, file_read_fds,  num_file_used, &total_file_fd);
    map_descriptor_sets(__clib4, write_fds,  num_fds, socket_write_fds,  num_socket_used, &total_socket_fd, file_write_fds, num_file_used, &total_file_fd);
    map_descriptor_sets(__clib4, except_fds, num_fds, socket_except_fds, num_socket_used, &total_socket_fd, NULL,           0,             &total_file_fd);

    __stdio_unlock(__clib4);

    /* Wait for socket input? */
    if (total_socket_fd > 0) {
        SHOWMSG("we have to deal with sockets");

        /* Wait for file input, too? */
        if ((total_file_fd > 0) && (timeout == NULL || timeout->tv_sec > 0 || timeout->tv_usec > 0)) {
            struct DateStamp stop_when;
            struct timeval zero;
            ULONG break_mask;
            BOOL got_input;
            BOOL got_output;

            SHOWMSG("we also have to deal with files");

            /* We may poll the sockets and files several times in a row.
             * The results stored in the tables can tell only a single
             * story, though. This is why we need to keep a backup copy
             * of each table around, to be used for each iteration of
             * the loop in which the sockets and files are checked.
             */
            if (read_fds != NULL) {
                if (num_socket_used > 0) {
                    SHOWMSG("allocating backup read socket fd_set");

                    backup_socket_read_fds = allocate_fd_set(__clib4, num_socket_used, socket_read_fds);
                    if (backup_socket_read_fds == NULL)
                        goto out;
                }

                if (num_file_used > 0) {
                    SHOWMSG("allocating backup read file fd_set");

                    backup_file_read_fds = allocate_fd_set(__clib4, num_file_used, file_read_fds);
                    if (backup_file_read_fds == NULL)
                        goto out;
                }
            }

            if (write_fds != NULL) {
                if (num_socket_used > 0) {
                    SHOWMSG("allocating backup write socket fd_set");

                    backup_socket_write_fds = allocate_fd_set(__clib4, num_socket_used, socket_write_fds);
                    if (backup_socket_write_fds == NULL)
                        goto out;
                }

                if (num_file_used > 0) {
                    SHOWMSG("allocating backup write file fd_set");

                    backup_file_write_fds = allocate_fd_set(__clib4, num_file_used, file_write_fds);
                    if (backup_file_write_fds == NULL)
                        goto out;
                }
            }

            if (except_fds != NULL) {
                if (num_socket_used > 0) {
                    SHOWMSG("allocating backup except socket fd_set");

                    backup_socket_except_fds = allocate_fd_set(__clib4, num_socket_used, socket_except_fds);
                    if (backup_socket_except_fds == NULL)
                        goto out;
                }
            }

            /* We are going to poll all streams; for the timeout
             * feature to work, we absolutely must know when to
             * stop polling.
             *
             * Why aren't we using asynchronous DOS packets?
             * The answer is that once a packet is sent, you
             * cannot easily abort it. Polling is safer in
             * that respect. Yes, I know that ACTION_STACK
             * can be used to fake input to a console stream,
             * but I'd rather not rely upon it.
             */
            if (timeout != NULL) {
                struct DateStamp datestamp_timeout;

                DateStamp(&stop_when);

                add_dates(&stop_when, timeval_to_datestamp(&datestamp_timeout, timeout));
            } else {
                /* No timeout, poll until we are interrupted
                 * or get input from any of the files. It's
                 * not really necessary to initialize this
                 * timeval, but it keeps the compiler happy.
                 */
                memset(&stop_when, 0, sizeof(stop_when));
            }

            while (TRUE) {
                /* Check for break signal. */
                __check_abort_f(__clib4);

                if (signal_mask_ptr) {
                    ULONG signals = CheckSignal(*signal_mask_ptr);
                    if (signals) {
                        *signal_mask_ptr &= signals;
                        result = 0;
                        break;
                    }
                }

                /* Delay for a tick to avoid busy-waiting. */
                Delay(1);

                /* This tells WaitSelect() to poll the sockets for input. */
                zero.tv_sec = 0;
                zero.tv_usec = 0;

                /* Signals to stop on; we want to stop when a break signal arrives. */
                break_mask = signal_mask;

                if (__clib4->__check_abort_enabled)
                    break_mask |= __clib4->__break_signal_mask;

                /* Check for socket input. */
                result = __WaitSelect(total_socket_fd, socket_read_fds, socket_write_fds, socket_except_fds, (struct timeval *) &zero, &break_mask);

                /* Stop if a break signal arrives. */
                if ((result < 0 && __get_errno() == EINTR) || FLAG_IS_SET(break_mask, __clib4->__break_signal_mask)) {
                    SetSignal(__clib4->__break_signal_mask, __clib4->__break_signal_mask);
                    __check_abort_f(__clib4);
                }

                if (0 == result && signal_mask_ptr) {
                    *signal_mask_ptr = break_mask & ~__clib4->__break_signal_mask;
                    break;
                }

                /* Stop if the return value from WaitSelect() is negative (timeout, abort or serious error). */
                if (result < 0) {
                    /* Update the signal mask if we received an interesting
                       signal. */
                    if (signal_mask_ptr != NULL && (break_mask & signal_mask) != 0)
                        (*signal_mask_ptr) = signal_mask & break_mask;

                    break;
                }

                /* Check all files for input. We also poll
                 * them for input, but each with a little
                 * delay of about 1 microsecond. We stop
                 * as soon as we find one file that has
                 * input in it.
                 */
                for (i = 0; i < total_file_fd; i++) {
                    got_input = got_output = FALSE;

                    fd = get_file_descriptor(__clib4, i);
                    if (fd != NULL) {
                        if (file_read_fds != NULL && FD_ISSET(i, file_read_fds)) {
                            if (FLAG_IS_SET(fd->fd_Flags, FDF_READ)) {
                                /* Is this a poll socket? */
                                if (FLAG_IS_SET(fd->fd_Flags, FDF_POLL)) {
                                    if (WaitForChar(Input(), 1)) {
                                        got_input = TRUE;
                                    }
                                }
                                /* Does this one have input? */
                                else if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE)) {
                                    if (i == STDIN_FILENO) {
                                        /* For STDIN stream, ask input. */
                                        if (WaitForChar(Input(), 1)) {
                                            got_input = TRUE;
                                        }
                                    }
                                    else {
                                        /* For an interactive stream, we simply ask. */
                                        if (WaitForChar(fd->fd_File, 1)) {
                                            got_input = TRUE;
                                        }
                                    }
                                } else {
                                    struct ExamineData *fib;

                                    /* For a file we check how much data is now in the file and
                                       compare it against the current file position. If there's
                                       unread data in the file, we will be able to read from it.
                                       For pipes, any data reported to be in the "file" indicates
                                       that there is something worth reading available. */
                                    fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
                                    if (fib) {
                                        if (FLAG_IS_SET(fd->fd_Flags, FDF_CACHE_POSITION)) {
                                            /* Is there new data to read? */
                                            if ((ULONG) fib->FileSize > fd->fd_Position)
                                                got_input = TRUE;
                                        } else {
                                            /* Does the pipe contain any data to read? */
                                            if (fib->FileSize != 0)
                                                got_input = TRUE;
                                        }
                                    }
                                    if (fib != NULL) {
                                        FreeDosObject(DOS_EXAMINEDATA, fib);
                                    }
                                }
                            }
                        }

                        if (file_write_fds != NULL && FD_ISSET(i, file_write_fds)) {
                            /* If this is a writable stream, assume that we can
                             * write to it. ZZZ AmigaDOS needs a method to check
                             * if the write will block.
                             */
                            if (FLAG_IS_SET(fd->fd_Flags, FDF_WRITE)) {
                                got_output = TRUE;
                            }
                            if (FLAG_IS_SET(fd->fd_Flags, FDF_POLL)) {
                                got_output = TRUE;
                            }
                        }
                    }

                    if (got_input || got_output) {
                        /* Mark one more descriptor as having input/output. */
                        result++;
                    }

                    if (file_read_fds != NULL && NOT got_input) {
                        FD_CLR(i, file_read_fds);
                    }

                    if (file_write_fds != NULL && NOT got_output) {
                        FD_CLR(i, file_write_fds);
                    }
                }

                /* Did we receive a stop signal? */
                if (signal_mask_ptr != NULL && (break_mask & signal_mask) != 0) {
                    /* Leave the interesting signal mask bits set. */
                    (*signal_mask_ptr) = signal_mask & break_mask;
                    break;
                }

                /* Did we get any input? If so, stop polling. */
                if (result > 0)
                    break;

                /* If a timeout was set, check if we are already beyond the point of time when we should have stopped polling. */
                if (timeout != NULL) {
                    struct DateStamp now;
                    DateStamp(&now);

                    if (CompareDates(&now, &stop_when) <= 0)
                        break;
                }

                /* No I/O ready yet. Restore the sets and retry... */
                copy_fd_set(socket_read_fds, backup_socket_read_fds, num_socket_used);
                copy_fd_set(socket_write_fds, backup_socket_write_fds, num_socket_used);
                copy_fd_set(socket_except_fds, backup_socket_except_fds, num_socket_used);

                copy_fd_set(file_read_fds, backup_file_read_fds, num_file_used);
                copy_fd_set(file_write_fds, backup_file_write_fds, num_file_used);
            }
        } else {
            ULONG break_mask;

            break_mask = signal_mask;

            if (__clib4->__check_abort_enabled)
                break_mask |= __clib4->__break_signal_mask;

            result = __WaitSelect(total_socket_fd, socket_read_fds, socket_write_fds, socket_except_fds, (struct timeval *) timeout, &break_mask);
            if ((result < 0 && __get_errno() == EINTR) || FLAG_IS_SET(break_mask, __clib4->__break_signal_mask)) {
                SetSignal(__clib4->__break_signal_mask, __clib4->__break_signal_mask);
                __check_abort_f(__clib4);
            }

            if (signal_mask_ptr != NULL && (break_mask & signal_mask) != 0)
                (*signal_mask_ptr) = signal_mask & break_mask;
        }
    } else {
        struct DateStamp stop_when;
        BOOL got_input;
        BOOL got_output;

        SHOWMSG("we have to deal with files");
        if (num_file_used > 0) {
            if (read_fds != NULL) {
                SHOWMSG("allocating backup file read fd_set");

                backup_file_read_fds = allocate_fd_set(__clib4, num_file_used, file_read_fds);
                if (backup_file_read_fds == NULL)
                    goto out;
            }

            if (write_fds != NULL) {
                SHOWMSG("allocating backup file write fd_set");

                backup_file_write_fds = allocate_fd_set(__clib4, num_file_used, file_write_fds);
                if (backup_file_write_fds == NULL)
                    goto out;
            }
        }

        if (timeout != NULL && (timeout->tv_sec > 0 || timeout->tv_usec > 0)) {
            struct DateStamp datestamp_timeout;
            DateStamp(&stop_when);

            add_dates(&stop_when, timeval_to_datestamp(&datestamp_timeout, timeout));
        }
        else {
            memset(&stop_when, 0, sizeof(stop_when));
        }

        while (TRUE) {
            __check_abort_f(__clib4);

            Delay(1);

            result = 0;
            for (i = 0; i < total_file_fd; i++) {
                got_input = got_output = FALSE;

                fd = get_file_descriptor(__clib4, i);
                if (fd != NULL) {
                    if (file_read_fds != NULL && FD_ISSET(i, file_read_fds)) {
                        if (FLAG_IS_SET(fd->fd_Flags, FDF_READ)) {
                            BPTR readFile = i == STDIN_FILENO ? Input() : fd->fd_File;
                            SHOWVALUE(FLAG_IS_SET(fd->fd_Flags, FDF_TERMIOS));
                            /* Check first if this is a POLL/TERMIOS FD
                             * In this case don't wait for char
                            */
                            if (FLAG_IS_SET(fd->fd_Flags, FDF_POLL)) {
                                SHOWVALUE("FLAG_IS_SET(fd->fd_Flags, FDF_POLL)");
                                got_input = TRUE;
                            }
                            else if (FLAG_IS_SET(fd->fd_Flags, FDF_TERMIOS)) {
                                SHOWVALUE("FLAG_IS_SET(fd->fd_Flags, FDF_TERMIOS");
                                struct termios *tios = fd->fd_Aux;

                                if (WaitForChar(readFile, 1)) {
                                    got_input = TRUE;
                                }
                            }
                            else if (FLAG_IS_SET(fd->fd_Flags, FDF_NON_BLOCKING)) {
                                SHOWVALUE("FLAG_IS_SET(fd->fd_Flags, FDF_NON_BLOCKING");
                                SHOWVALUE(i);
                                got_input = TRUE;
                            }
                            else if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE)) {
                                SHOWVALUE("FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE");
                                SHOWVALUE(i);
                                if (WaitForChar(readFile, 1)) {
                                    got_input = TRUE;
                                }
                            } else {
                                struct ExamineData *fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
                                if (fib != NULL) {
                                    if (FLAG_IS_SET(fd->fd_Flags, FDF_CACHE_POSITION)) {
                                        if ((ULONG) fib->FileSize > fd->fd_Position)
                                            got_input = TRUE;
                                    } else {
                                        if (fib->FileSize != 0)
                                            got_input = TRUE;
                                    }
                                    FreeDosObject(DOS_EXAMINEDATA, fib);
                                }
                            }
                        }
                    }

                    if (file_write_fds != NULL && FD_ISSET(i, file_write_fds)) {
                        if (FLAG_IS_SET(fd->fd_Flags, FDF_WRITE)) {
                            assert(FLAG_IS_CLEAR(fd->fd_Flags, FDF_IS_SOCKET));
                            got_output = TRUE;
                        }
                    }
                }
                SHOWVALUE(got_input);
                SHOWVALUE(got_output);

                if (file_read_fds != NULL && NOT got_input) {
                    FD_CLR(i, file_read_fds);
                }

                if (file_write_fds != NULL && NOT got_output) {
                    FD_CLR(i, file_write_fds);
                }

                if (got_input || got_output)
                    result++;
            }

            /* Check for a stop signal. */
            if (signal_mask != 0 && (SetSignal(0, 0) & signal_mask) != 0) {
                /* Remember which signal bits were set, and clear the
                   signal mask. Note that if the signal mask includes the
                   standard break signal bit, then we must not clear the
                   break signal. The ^C checking depends upon it to
                   remain set. */
                (*signal_mask_ptr) = signal_mask & SetSignal(0, signal_mask & ~__clib4->__break_signal_mask);
                break;
            }

            if (result > 0)
                break;

            if (timeout != NULL && (timeout->tv_sec > 0 || timeout->tv_usec > 0)) {
                struct DateStamp now;
                DateStamp(&now);

                if (CompareDates(&now, &stop_when) <= 0)
                    break;
            }

            copy_fd_set(file_read_fds, backup_file_read_fds, num_file_used);
            copy_fd_set(file_write_fds, backup_file_write_fds, num_file_used);
        }
    }

    /* The descriptor sets remain unchanged in
     * case of error.
     */
    if (result >= 0) {
        SHOWMSG("remapping fd_sets");

        __stdio_lock(__clib4);

        remap_descriptor_sets(__clib4, socket_read_fds, total_socket_fd, file_read_fds, total_file_fd, read_fds, num_fds);
        remap_descriptor_sets(__clib4, socket_write_fds, total_socket_fd, file_write_fds, total_file_fd, write_fds, num_fds);
        remap_descriptor_sets(__clib4, socket_except_fds, total_socket_fd, NULL, 0, except_fds, num_fds);

        __stdio_unlock(__clib4);
    }

    __check_abort_f(__clib4);

out:

    free_fd_set(__clib4, socket_read_fds);
    free_fd_set(__clib4, socket_write_fds);
    free_fd_set(__clib4, socket_except_fds);

    free_fd_set(__clib4, file_read_fds);
    free_fd_set(__clib4, file_write_fds);

    free_fd_set(__clib4, backup_socket_read_fds);
    free_fd_set(__clib4, backup_socket_write_fds);
    free_fd_set(__clib4, backup_socket_except_fds);

    free_fd_set(__clib4, backup_file_read_fds);
    free_fd_set(__clib4, backup_file_write_fds);

    RETURN(result);
    return (result);
}
