/*
 * $Id: usergroup_getgrnam.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

struct group *
getgrnam(const char *name)
{
	struct group *result = NULL;

	ENTER();

	SHOWSTRING(name);

	assert(name != NULL);
	assert(__UserGroupBase != NULL);

    if (name == NULL)
    {
        SHOWMSG("invalid name parameter");

        __set_errno(EFAULT);
        goto out;
    }

	result = __getgrnam((STRPTR)name);

out:

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
