/*
 * $Id: stat_umask.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

mode_t NOCOMMON __current_umask = S_IWGRP | S_IWOTH;

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

mode_t
umask(mode_t new_mask)
{
	mode_t result;

	ENTER();

	SHOWVALUE(new_mask);

	assert(__UserGroupBase != NULL);

	result = __getumask();

	__current_umask = new_mask & (S_IRWXU | S_IRWXG | S_IRWXO);

	__umask(__current_umask);

	RETURN(result);
	return(result);
}
