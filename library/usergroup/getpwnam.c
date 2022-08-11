/*
 * $Id: usergroup_getpwnam.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

struct passwd *
getpwnam(const char *name)
{
	struct passwd *result = NULL;

	ENTER();

	SHOWSTRING(name);

	assert(name != NULL);
	assert(__UserGroupBase != NULL);

    if (name == NULL)
    {
        SHOWMSG("invalid name");

        __set_errno(EFAULT);
        goto out;
    }

	result = __getpwnam((STRPTR)name);

out:

    __check_abort();

	RETURN(result);
	return (result);
}
