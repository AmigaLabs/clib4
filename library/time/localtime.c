/*
 * $Id: time_localtime.c,v 1.5 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

struct tm *
localtime(const time_t *t)
{
	static struct tm tm;

	struct tm * result;

	ENTER();

	result = localtime_r(t,&tm);

	RETURN(result);
	return(result);
}
