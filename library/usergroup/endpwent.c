/*
 * $Id: usergroup_endpwent.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/****************************************************************************/

#ifdef endpwent
#undef endpwent
#endif /* endpwent */

/****************************************************************************/

void endpwent(void)
{
	ENTER();

	assert(__UserGroupBase != NULL);

	__endpwent();

    __check_abort();

	LEAVE();
}
