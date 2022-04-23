/*
 * $Id: resource_setrlimit.c,v 1.2 2006-07-28 14:37:27 clib2devs Exp $
*/

#include <sys/resource.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
setrlimit(int resource, const struct rlimit *rlp)
{
	int ret = -1;

    ENTER();

    SHOWVALUE(resource);
    SHOWPOINTER(rlp);

    if (rlp == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	switch (resource)
	{
        case RLIM_VMEM:
        case RLIM_CORE:
        case RLIM_CPU:
        case RLIM_DATA:
        case RLIM_FSIZE:
        case RLIM_NOFILE:
        case RLIM_STACK: /* TODO: See if it might be possible to set the stacksize here. */

            __set_errno(EPERM);
            goto out;

        default:

            __set_errno(EINVAL);
            goto out;
	}

	ret = 0;

out:

    RETURN(ret);
	return (ret);
}
