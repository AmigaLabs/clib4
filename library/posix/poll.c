/*
 * $Id: poll.c,v 1.1 2024-07-04 11:50:0 clib4devs Exp $
*/

#ifndef  _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

static int
map_poll_spec(struct pollfd *pArray, nfds_t n_fds, fd_set *pReadSet, fd_set *pWriteSet, fd_set *pExceptSet, BOOL *stdin_set_raw) {
    register nfds_t i;             /* loop control */
    register struct pollfd *pCur;  /* current array element */
    register int max_fd = -1;      /* return value */
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    *stdin_set_raw = FALSE;

    SHOWPOINTER(pArray);
    SHOWVALUE(n_fds);
    SHOWPOINTER(pReadSet);
    SHOWPOINTER(pWriteSet);
    SHOWPOINTER(pExceptSet);

    /*
       Map the poll() structures into the file descriptor sets required
       by select().
    */
    for (i = 0, pCur = pArray; i < n_fds; i++, pCur++) {
        /* Skip any bad FDs in the array. */

        if (pCur->fd < 0)
            continue;

        struct fd *fd = __get_file_descriptor(__clib4, pCur->fd);
        if (fd == NULL) {
            pCur->revents = POLLNVAL;
            continue;
        }

        if (pCur->events & POLLIN) {
            /* "Input Ready" notification desired. */
            FD_SET(pCur->fd, pReadSet);
        }

        if (pCur->events & POLLOUT) {
            /* "Output Possible" notification desired. */
            FD_SET(pCur->fd, pWriteSet);
        }

        if (pCur->events & POLLPRI) {
            /* "Exception Occurred" notification desired. (Exceptions include out of band data.) */
            FD_SET(pCur->fd, pExceptSet);
        }

        /* Always set FDF_POLL so that select_signal knows this descriptor
           is under poll control and does not call ExamineObjectTags on
           something that is not a real file. */
        SET_FLAG(fd->fd_Flags, FDF_POLL);

        /* Use the file descriptor number (pCur->fd), not the BPTR fd_File. */
        if (pCur->fd >= STDIN_FILENO && pCur->fd <= STDERR_FILENO) {
            if (pCur->fd == STDIN_FILENO && FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE)) {
                /* Set RAW mode so WaitSelect can detect individual keystrokes.
                 * Track this so we can restore cooked mode after poll returns. */
                BPTR file = __resolve_fd_file(fd);
                if (file != BZERO) {
                    SetMode(file, DOSTRUE);
                    *stdin_set_raw = TRUE;
                }

                SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
            }
            else if (pCur->fd != STDIN_FILENO) {
                SET_FLAG(fd->fd_Flags, FDF_WRITE | FDF_IS_INTERACTIVE);
            }
        }

        max_fd = MAX(max_fd, pCur->fd);
    }

    RETURN(max_fd);
    return max_fd;
}

static struct timeval *
map_timeout(int poll_timeout, struct timeval *pSelTimeout) {
    struct timeval *pResult;

    /*
       Map the poll() timeout value into a select() timeout.  The possible
       values of the poll() timeout value, and their meanings, are:

       VALUE	MEANING

       -1	wait indefinitely (until signal occurs)
        0	return immediately, don't block
       >0	wait specified number of milliseconds

       select() uses a "struct timeval", which specifies the timeout in
       seconds and microseconds, so the milliseconds value has to be mapped
       accordingly.
    */

    assert(pSelTimeout != (struct timeval *) NULL);

    switch (poll_timeout) {
        case -1:
            /*
               A NULL timeout structure tells select() to wait indefinitely.
            */
            pResult = (struct timeval *) NULL;
            break;
        case 0:
            /*
               "Return immediately" (test) is specified by all zeros in
               a timeval structure.
            */
            pSelTimeout->tv_sec = 0;
            pSelTimeout->tv_usec = 0;
            pResult = pSelTimeout;
            break;
        default:
            /* Wait the specified number of milliseconds. */
            pSelTimeout->tv_sec = poll_timeout / 1000;  /* get seconds */
            poll_timeout %= 1000;                       /* remove seconds */
            pSelTimeout->tv_usec = poll_timeout * 1000; /* get microseconds */
            pResult = pSelTimeout;
            break;
    }

    return pResult;
}

static void
map_select_results(struct pollfd *pArray, unsigned long n_fds, fd_set *pReadSet, fd_set *pWriteSet, fd_set *pExceptSet) {
    register unsigned long i;      /* loop control */
    register struct pollfd *pCur;  /* current array element */

    for (i = 0, pCur = pArray; i < n_fds; i++, pCur++) {
        /* Skip any bad FDs in the array. */
        if (pCur->fd < 0)
            continue;

        pCur->revents = 0;
        if (FD_ISSET(pCur->fd, pExceptSet)) {
            pCur->revents |= POLLPRI;
        }

        if (FD_ISSET(pCur->fd, pReadSet)) {
            pCur->revents |= POLLIN;
        }

        if (FD_ISSET(pCur->fd, pWriteSet)) {
            pCur->revents |= POLLOUT;
        }
    }

    return;
}

/*
 * poll_cleanup — Restore console mode and fd flags after select returns.
 *
 * If map_poll_spec set stdin to RAW mode, restore cooked (canonical) mode.
 * Also clear the temporary FDF_POLL and FDF_NON_BLOCKING flags.
 */
static void
poll_cleanup(struct pollfd *pArray, nfds_t n_fds, BOOL stdin_was_set_raw) {
    struct _clib4 *__clib4 = __CLIB4;
    nfds_t i;
    struct pollfd *pCur;

    for (i = 0, pCur = pArray; i < n_fds; i++, pCur++) {
        if (pCur->fd < 0)
            continue;

        struct fd *fd = __get_file_descriptor(__clib4, pCur->fd);
        if (fd == NULL)
            continue;

        CLEAR_FLAG(fd->fd_Flags, FDF_POLL);

        if (pCur->fd == STDIN_FILENO && stdin_was_set_raw) {
            /* Restore cooked (canonical) mode on stdin. */
            BPTR file = __resolve_fd_file(fd);
            if (file != BZERO)
                SetMode(file, DOSFALSE);

            CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
        }
    }
}

int
__poll(struct pollfd *fds, nfds_t nfds, int timeout, uint32_t *signals) {
    fd_set read_descs;                          /* input file descs */
    fd_set write_descs;                         /* output file descs */
    fd_set except_descs;                        /* exception descs */
    struct timeval stime;                       /* select() timeout value */
    int ready_descriptors;                      /* function result */
    int max_fd;                                 /* maximum fd value */
    struct timeval *pTimeout;                   /* actually passed */
    BOOL stdin_set_raw = FALSE;

    if ((fds == NULL) && (nfds != 0)) {
        __set_errno(EFAULT);
        return -1;
    }

    FD_ZERO(&read_descs);
    FD_ZERO(&write_descs);
    FD_ZERO(&except_descs);

    memset(&stime, 0, sizeof(stime));

    /* Map the poll() file descriptor list in the select() data structures. */
    max_fd = map_poll_spec(fds, nfds, &read_descs, &write_descs, &except_descs, &stdin_set_raw);

    /* Map the poll() timeout value in the select() timeout structure. */
    pTimeout = map_timeout(timeout, &stime);

    /* Make the select() call. */
    if (signals)
        ready_descriptors = waitselect(max_fd + 1, &read_descs, &write_descs, &except_descs, pTimeout, (long unsigned int *) signals);
    else
        ready_descriptors = select(max_fd + 1, &read_descs, &write_descs, &except_descs, pTimeout);

    if (ready_descriptors >= 0) {
        map_select_results(fds, nfds, &read_descs, &write_descs, &except_descs);
    }

    /* Restore stdin cooked mode and clear temporary flags. */
    poll_cleanup(fds, nfds, stdin_set_raw);

    return ready_descriptors;
}

int
poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    return __poll(fds, nfds, timeout, 0);
}