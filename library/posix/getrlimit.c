/*
 * $Id: resource_getrlimit.c,v 1.2 2006-07-28 14:37:27 clib4devs Exp $
*/

#include <sys/resource.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
getrlimit(int resource, struct rlimit *rlp) {
    struct Task *self;
    int ret = -1;
    rlim_t l;

    ENTER();

    SHOWVALUE(resource);
    SHOWPOINTER(rlp);

    if (rlp == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    switch (resource) {
        case RLIM_VMEM:

            rlp->rlim_cur = RLIM_INFINITY;
            rlp->rlim_max = RLIM_INFINITY;
            break;

        case RLIM_CORE:    /* Coredumps are not supported. */

            rlp->rlim_cur = 0;
            rlp->rlim_max = 0;
            break;

        case RLIM_CPU:

            rlp->rlim_cur = RLIM_INFINITY;
            rlp->rlim_max = RLIM_INFINITY;
            break;

        case RLIM_DATA:
            l = AvailMem(MEMF_TOTAL | MEMF_VIRTUAL);
            rlp->rlim_cur = l;
            rlp->rlim_max = l;
            break;

        case RLIM_FSIZE:

            rlp->rlim_cur = RLIM_INFINITY;    /* Use RLIM_INFINITY in case we have a 64-bit fs. pathconf() can be more precise. */
            rlp->rlim_max = RLIM_INFINITY;
            break;

        case RLIM_NOFILE:

            rlp->rlim_cur = RLIM_INFINITY;
            rlp->rlim_max = RLIM_INFINITY;
            break;

        case RLIM_STACK:    /* Return current stacksize. */

            self = FindTask(NULL);

            l = (char *) self->tc_SPUpper - (char *) self->tc_SPLower;

            rlp->rlim_cur = l;
            rlp->rlim_max = RLIM_INFINITY;

            break;

        default:

            __set_errno(EINVAL);
            goto out;
    }

    ret = 0;

out:

    RETURN(ret);
    return (ret);
}
