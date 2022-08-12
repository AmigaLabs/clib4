/*
 * $Id: usergroup_getpwent.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

struct passwd *
getpwent(void)
{
	struct passwd *result;

	ENTER();

	assert(__UserGroupBase != NULL);

	result = __getpwent();

    __check_abort();

	RETURN(result);
	return (result);
}
