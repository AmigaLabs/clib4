/*
 * $Id: usergroup_getpgrp.c,v 1.0 2021-02-14 18:17:00 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

pid_t
getpgrp(void)
{
	pid_t result;

	ENTER();

	assert(__UserGroupBase != NULL);

	result = __getpgrp();

	if (__check_abort_enabled)
		__check_abort();

	RETURN(result);
	return (result);
}
