/*
 * $Id: time_clock.c,v 1.7 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/****************************************************************************/

static struct DateStamp start_time;

/****************************************************************************/

CLIB_CONSTRUCTOR(clock_init)
{
	ENTER();

	/* Remember when this program was started. */
	DateStamp(&start_time);

	LEAVE();

	CONSTRUCTOR_SUCCEED();
}

/****************************************************************************/

clock_t
clock(void)
{
	struct timeval now;
	uint64 usec_now, usec_start;
	clock_t result;

	ENTER();

	/* Get the current time. */
	gettimeofday(&now, NULL);

	usec_now = now.tv_sec * 1000000ULL + now.tv_usec;
	usec_start = ((struct TimeVal *)(&__global_clib2->clock))->Seconds * 1000000ULL + ((struct TimeVal *)(&__global_clib2->clock))->Microseconds;

	/* Subtract the start time from the current time. */
	usec_now -= usec_start;

	result = (clock_t)(usec_now * CLK_TCK / 1000000);

	RETURN(result);
	return result;
}
