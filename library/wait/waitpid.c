/*
 * $Id: wait_wait.c,v 1.0 2023-06-09 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options) {
    if (options != WNOHANG) {
        __set_errno(EINVAL);
        return -1;
    }

    if (pid )
}