/*
 * $Id: usergroup_setgid.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

int setgid(gid_t id)
{
	int result;

	ENTER();

	SHOWVALUE(id);

	assert(__UserGroupBase != NULL);

	if (__root_mode)
	{
		__root_gid = id;

		result = OK;
	}
	else
	{
		result = __setgid((LONG)id);
	}

    __check_abort();

	RETURN(result);
	return (result);
}
