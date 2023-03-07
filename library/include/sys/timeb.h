/*
 * $Id: timeb.h,v 1.3 2006-07-28 14:02:32 clib2devs Exp $
*/

#ifndef	_SYS_TIMEB_H
#define	_SYS_TIMEB_H

#ifndef _TIME_H
#include <time.h>	/* For the definition of time_t */
#endif /* _TIME_H */

#include <features.h>

__BEGIN_DECLS

struct timeb
{
	time_t			time;
	unsigned short	millitm;
	short			timezone;
	short			dstflag;
};

extern int ftime(struct timeb *);

__END_DECLS

#endif /* _SYS_TIMEB_H */
