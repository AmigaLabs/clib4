/*
 * $Id: usergroup_setegid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

int setegid(gid_t g)
{
	int result;

	ENTER();

	SHOWVALUE(g);

	assert(__UserGroupBase != NULL);

	result = setregid((ULONG)-1, (ULONG)g);

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
