/*
 * $Id: utsname.h,v 1.4 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef	_SYS_UTSNAME_H
#define	_SYS_UTSNAME_H

#include <features.h>

__BEGIN_DECLS

#define	SYS_NMLN 32

struct utsname
{
	char sysname[SYS_NMLN];		/* OS Name ("AmigaOS") */
	char nodename[SYS_NMLN];	/* Usually Hostname, but there appears to be no standard.  */
	char release[SYS_NMLN];		/* Release e.g. "51.1" (The Version of version.library) */
	char version[SYS_NMLN];		/* Version e.h. "4.0" (OS Version) */
	char machine[SYS_NMLN];		/* Machine e.g. "m68k" or "ppc" */
};

extern int uname(struct utsname *);

__END_DECLS

#endif /* _SYS_UTSNAME_H */
