/*
 * $Id: time_weekday.c,v 1.2 2006-01-08 12:04:27 clib2devs Exp $
*/

/****************************************************************************/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/****************************************************************************/

int
__calculate_weekday(int year,int month,int day)
{
	/* This algorithm comes from <http://klausler.com/new-dayofweek.html>. */
	static const char month_skew[12] =
	{
		0,3,3,6,
		1,4,6,2,
		5,0,3,5
	};

	int sum;

	/* Subtract 1900 from the year. */
	sum = year - 1900;

	/* To that number, add one fourth of itself, discarding
	   any remainder. This sum is the year's skew value. */
	sum += sum / 4;

	/* If the month in question is January or February in a leap
	   year, subtract 1 from the sum. */
	if(month == 1 || month == 2)
	{
		int leap_year_adjust;

		/* Figure out if the year is a leap year. */
		if((year % 4) != 0)
			leap_year_adjust = 0;
		else if ((year % 400) == 0)
			leap_year_adjust = 1;
		else if ((year % 100) == 0)
			leap_year_adjust = 0;
		else
			leap_year_adjust = 1;

		sum -= leap_year_adjust;
	}

	/* Add the month's skew value from the table. */
	sum += day + month_skew[month-1];

	/* The sum is the number of days after Sunday on which the date falls. */
	return(sum % 7);
}
