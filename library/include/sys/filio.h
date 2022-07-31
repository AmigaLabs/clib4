/*
 * $Id: filio.h,v 1.4 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _SYS_FILIO_H
#define _SYS_FILIO_H

#ifndef	_SYS_IOCCOM_H
#include <sys/ioccom.h>
#endif /* _SYS_IOCCOM_H */

#include <features.h>

__BEGIN_DECLS

/* Generic file-descriptor ioctl's. */
#define	FIOCLEX		 _IO('f', 1)		/* set close on exec on fd */
#define	FIONCLEX	 _IO('f', 2)		/* remove close on exec */
#define	FIONREAD	_IOR('f', 127, int)	/* get # bytes to read */
#define	FIONBIO		_IOW('f', 126, int)	/* set/clear non-blocking i/o */
#define	FIOASYNC	_IOW('f', 125, int)	/* set/clear async i/o */
#define	FIOSETOWN	_IOW('f', 124, int)	/* set owner */
#define	FIOGETOWN	_IOR('f', 123, int)	/* get owner */

__END_DECLS

#endif /* _SYS_FILIO_H */
