/*
 * $Id: usergroup_geteuid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

#ifdef geteuid
#undef geteuid
#endif /* geteuid */

/****************************************************************************/

uid_t geteuid(void)
{
	uid_t result;

	ENTER();

	assert(__UserGroupBase != NULL);

	if (__root_mode)
	{
		result = __root_euid;
	}
	else
	{
		result = __geteuid();
	}

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
