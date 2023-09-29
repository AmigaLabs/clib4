/*
 * $Id: unistd_flock.c,v 1.0 2022-08-21 12:04:22 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
lockf(int file_descriptor, int function, off_t size) {
	struct flock l;
	int result = -1;
	int cmd;

	l.l_whence = SEEK_CUR;
	l.l_start = 0;
	l.l_len = size;

	switch (function) {
        case F_ULOCK:

            cmd = F_SETLK;
            l.l_type = F_UNLCK;
            break;

        case F_LOCK:

            cmd = F_SETLKW;
            l.l_type = F_WRLCK;
            break;

        case F_TLOCK:

            cmd = F_SETLK;
            l.l_type = F_WRLCK;
            break;

        case F_TEST:

            l.l_type = F_WRLCK;

            if (fcntl(file_descriptor, F_GETLK, &l) == -1)
                goto out;

            if (l.l_pid == getpid() || l.l_type == F_UNLCK) {
                result = 0;
                goto out;
            }

            __set_errno(EAGAIN);
            goto out;

        default:

            __set_errno(EINVAL);
            goto out;
	}

	result = fcntl(file_descriptor, cmd, &l);

out:

	return (result);
}
