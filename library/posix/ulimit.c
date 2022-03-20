/*
 * $Id: ulimit_ulimit.c,v 1.2 2006-07-28 14:37:27 clib2devs Exp $
*/

#include <ulimit.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

long
ulimit(int cmd, long newlim) {
    long ret = -1;
    (void) (newlim);

    switch (cmd) {
        case UL_GETFSIZE:
            /* Maximum number of 512-byte blocks in a file. Largefile aware programs should not use ulimit() anyway. */
            ret = (0x7fffffffL >> 9) - 1L; /* Max Filesize/512 - 1 */
            break;
        case UL_GMEMLIM:
            /* Which flags are appropriate for AvailMem()? */
            ret = AvailMem(MEMF_TOTAL | MEMF_VIRTUAL);
            break;
        case UL_GDESLIM:    /* No limit, so we just return a reasonably large value. */
            ret = 1024;
            break;
        case UL_SETFSIZE:    /* Not supported */
            __set_errno(EPERM);
            goto out;
        default:
            __set_errno(EINVAL);
            goto out;
    }
out:
    return (ret);
}
