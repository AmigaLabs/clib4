/*
 * $Id: mman_mlockall.c,v 1.0 2026-04-14 00:00:00 clib4devs Exp $
 *
 * mlockall() - lock the entire virtual address space of the process
 *
 * POSIX.1-2001 / SVr4.  See man mlock(2).
 *
 * On AmigaOS 4 there is no virtual memory / swap subsystem, so all
 * allocated memory is always physically resident in RAM.  This function
 * validates the flags and returns success as a no-op.
 *
 * Errors:
 *   EINVAL  Unknown flags, or MCL_ONFAULT without MCL_CURRENT/MCL_FUTURE.
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>

int
mlockall(int flags)
{
    ENTER();

    SHOWVALUE(flags);

    /* Validate: only known flags */
    if ((unsigned int)flags & ~(unsigned int)(MCL_CURRENT | MCL_FUTURE | MCL_ONFAULT)) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    /* MCL_ONFAULT requires MCL_CURRENT or MCL_FUTURE */
    if ((flags & MCL_ONFAULT) && !(flags & (MCL_CURRENT | MCL_FUTURE))) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    /* AmigaOS 4 has no virtual memory / swap: all memory is always
     * resident, so mlockall() is a successful no-op. */
    RETURN(0);
    return 0;
}
