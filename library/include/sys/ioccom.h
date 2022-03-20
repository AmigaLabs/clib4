/*
 * $Id: ioccom.h,v 1.4 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _SYS_IOCCOM_H
#define _SYS_IOCCOM_H

#include <features.h>

__BEGIN_DECLS

#define	IOCPARM_MASK	0x1fff		/* parameter length, at most 13 bits */
#define	IOCPARM_LEN(x)	(((x) >> 16) & IOCPARM_MASK)
#define	IOCBASECMD(x)	((x) & ~(IOCPARM_MASK << 16))
#define	IOCGROUP(x)		(((x) >> 8) & 0xff)

#define	IOCPARM_MAX	NBPG	/* max size of ioctl args, mult. of NBPG */

#define	IOC_VOID	(0x20000000UL)		/* no parameters */
#define	IOC_OUT		(0x40000000UL)		/* copy parameters out */
#define	IOC_IN		(0x80000000UL)		/* copy parameters in */
#define	IOC_INOUT	(IOC_IN|IOC_OUT)	/* copy paramters in and out */
#define	IOC_DIRMASK	(0xe0000000UL)		/* mask for IN/OUT/VOID */

#define	_IOC(inout,group,num,len) \
	(inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))

#define	_IO(g,n)		_IOC(IOC_VOID,	(g), (n), 0)
#define	_IOR(g,n,t)		_IOC(IOC_OUT,	(g), (n), sizeof(t))
#define	_IOW(g,n,t)		_IOC(IOC_IN,	(g), (n), sizeof(t))
#define	_IOWR(g,n,t)	_IOC(IOC_INOUT,	(g), (n), sizeof(t))

__END_DECLS

#endif /* _SYS_IOCCOM_H */
