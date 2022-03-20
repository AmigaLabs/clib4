/*
 * $Id: usergroup_getpwuid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

struct passwd *
getpwuid(uid_t uid)
{
	struct passwd *result;

	ENTER();

	assert(__UserGroupBase != NULL);

	SHOWVALUE(uid);

	result = __getpwuid((LONG)uid);

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
