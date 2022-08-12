/*
 * $Id: usergroup_getgid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

#ifdef getgid
#undef getgid
#endif /* getgid */

/****************************************************************************/

gid_t getgid(void)
{
	gid_t result;

	ENTER();

	assert(__UserGroupBase != NULL);

	if (__root_mode)
	{
		result = __root_gid;
	}
	else
	{
		result = __getgid();
	}

    __check_abort();

	RETURN(result);
	return (result);
}
