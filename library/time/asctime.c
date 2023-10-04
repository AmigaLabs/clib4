/*
 * $Id: time_asctime.c,v 1.5 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

char *
asctime(const struct tm *tm)
{
	static char buffer[40] = {0};

	char * result;

	ENTER();

	result = __asctime_r(tm,buffer,sizeof(buffer));

	RETURN(result);
	return(result);
}
