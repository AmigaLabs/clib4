/*
 * $Id: mman_munlockall.c,v 1.0 2026-04-14 00:00:00 clib4devs Exp $
 *
 * munlockall() - unlock the entire virtual address space of the process
 *
 * POSIX.1-2001 / SVr4.  See man mlock(2).
 *
 * On AmigaOS 4 there is no virtual memory / swap subsystem, so all
 * allocated memory is always physically resident in RAM.  This function
 * is a successful no-op.
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/mman.h>

int
munlockall(void)
{
    ENTER();

    /* AmigaOS 4 has no virtual memory / swap: all memory is always
     * resident, so munlockall() is a successful no-op. */
    RETURN(0);
    return 0;
}
