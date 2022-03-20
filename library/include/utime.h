/*
 * $Id: utime.h,v 1.7 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _UTIME_H
#define _UTIME_H

#include <features.h>

#ifndef _TIME_H
#include <time.h>
#endif /* _TIME_H */

__BEGIN_DECLS

struct utimbuf
{
	time_t actime;		/* Access time */
	time_t modtime;		/* Modification time */
};

extern int utime(const char * path_name,const struct utimbuf * times);

__END_DECLS

#endif /* _UTIME_H */
