/*
 * $Id: ulimit.h,v 1.2 2006-07-28 14:02:32 clib2devs Exp $
*/

#ifndef	_ULIMIT_H
#define	_ULIMIT_H

#include <features.h>

__BEGIN_DECLS

#define	UL_GETFSIZE	1
#define	UL_SETFSIZE	2
#define	UL_GMEMLIM	3
#define	UL_GDESLIM	4

extern long ulimit(int cmd,long newlimit);

__END_DECLS

#endif /* _ULIMIT_H */
