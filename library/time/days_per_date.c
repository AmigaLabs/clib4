/*
 * $Id: time_days_per_date.c,v 1.2 2006-01-08 12:04:27 clib4devs Exp $
*/

/****************************************************************************/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/****************************************************************************/

/* This calculates the number of days that have passed up to the
   given date. */
int
__calculate_days_per_date(int year,int month,int day)
{
	int result;

	month	= month + 9;
	year	= year - 1 + (month / 12);
	month	= (month % 12) * 306 + 5;

	result = (year * 365) + (year / 4) - (year / 100) + (year / 400) + (month / 10) + day - 1;

	return(result);
}
