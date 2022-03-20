/*
 * $Id: time_difftime.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/****************************************************************************/

double
difftime(time_t t1, time_t t0)
{
	double result;

	result = ((double)t1) - ((double)t0);

	return (result);
}
