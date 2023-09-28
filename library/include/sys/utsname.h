/*
 * $Id: utsname.h,v 1.5 2022-02-04 12:06:14 clib4devs Exp $
*/

#ifndef	_SYS_UTSNAME_H
#define	_SYS_UTSNAME_H

#include <features.h>

__BEGIN_DECLS

#define	SYS_NMLN 65

struct utsname
{
	char sysname[SYS_NMLN];		/* OS Name ("AmigaOS") */
	char nodename[SYS_NMLN];	/* Usually Hostname, but there appears to be no standard.  */
	char release[SYS_NMLN];		/* Release e.g. "51.1" (The Version of version.library) */
	char version[SYS_NMLN];		/* Version e.h. "4.0" (OS Version) */
	char machine[SYS_NMLN];		/* Machine e.g. "m68k" or "ppc" */
#ifdef _GNU_SOURCE
    char domainname[SYS_NMLN];
#else
    char __domainname[SYS_NMLN];
#endif
};

extern int uname(struct utsname *);

__END_DECLS

#endif /* _SYS_UTSNAME_H */
