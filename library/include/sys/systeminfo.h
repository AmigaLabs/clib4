/*
 * $Id: systeminfo.h,v 1.1 2006-07-28 14:02:32 clib2devs Exp $
*/

#ifndef _SYS_SYSTEMINFO_H
#define _SYS_SYSTEMINFO_H

#include <features.h>

__BEGIN_DECLS

/* Numbers are picked to be the same as for Solaris */

#define	SI_SYSNAME			1
#define	SI_HOSTNAME			2
#define	SI_RELEASE			3
#define	SI_VERSION			4
#define	SI_MACHINE			5
#define	SI_ARCHITECTURE		6
#define	SI_HW_SERIAL		7
#define	SI_HW_PROVIDER		8

#define	SI_SET_HOSTNAME		258

#define	SI_PLATFORM			513
#define	SI_ISALIST			514

/****************************************************************************/

extern long sysinfo(int cmd,char *buf,long buflen);

__END_DECLS

#endif /* _SYS_SYSTEMINFO_H */
