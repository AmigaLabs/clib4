/*
 * $Id: usergroup_getegid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

#ifdef getegid
#undef getegid
#endif /* getegid */

/****************************************************************************/

gid_t getegid(void)
{
	gid_t result;

	ENTER();

	assert(__UserGroupBase != NULL);

	if (__root_mode)
	{
		result = __root_egid;
	}
	else
	{
		result = __getegid();
	}

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
