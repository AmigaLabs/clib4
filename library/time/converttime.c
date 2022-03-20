/*
 * $Id: time_converttime.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/****************************************************************************/

struct tm *
__convert_time(ULONG seconds, LONG gmt_offset, struct tm * tm)
{
	DECLARE_UTILITYBASE();
	struct ClockData clock_data;
	struct tm * result;

	ENTER();

	assert( UtilityBase != NULL );

	/* We need to convert the time from Unix-style UTC
	 * back into Amiga style local time.
	 *
	 * First, the Unix time offset will have to go.
	 */
	if(seconds < UNIX_TIME_OFFSET)
		seconds = 0;
	else
		seconds -= UNIX_TIME_OFFSET;

	/* Now the local time offset will have to go. */
	seconds -= gmt_offset;

	/* Convert the number of seconds into a more useful format. */
	Amiga2Date(seconds, &clock_data);

	/* The 'struct clockdata' layout and contents are very similar
	 * to the 'struct tm' contents. We don't have to convert much,
	 * except for the 'tm.tm_yday' field below.
	 */
	tm->tm_sec		= clock_data.sec;
	tm->tm_min		= clock_data.min;
	tm->tm_hour		= clock_data.hour;
	tm->tm_mday		= clock_data.mday;
	tm->tm_mon		= clock_data.month - 1;
	tm->tm_year		= clock_data.year - 1900;
	tm->tm_wday		= clock_data.wday;
	tm->tm_isdst	= -1;

	/* Now figure out how many days have passed since January 1st. */
	tm->tm_yday = __calculate_days_per_date(clock_data.year,clock_data.month,clock_data.mday) - __calculate_days_per_date(clock_data.year,1,1);

	result = tm;

	RETURN(result);
	return(result);
}
