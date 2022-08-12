/*
 * $Id: usergroup_getgrgid.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

struct group *
getgrgid(gid_t gid)
{
	struct group *result;

	ENTER();

	assert(__UserGroupBase != NULL);

	result = __getgrgid((LONG)gid);

    __check_abort();

	RETURN(result);
	return (result);
}
