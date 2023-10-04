/*
 * $Id: uio.h,v 1.5 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif /* _SYS_TYPES_H */

#ifndef _STDDEF_H
#include <stddef.h>
#endif /* _STDDEF_H */

#include <features.h>

__BEGIN_DECLS

/*
 * The size of MAX_IOVEC is rather arbitrary since there is no kernel support
 * for vectored I/O and even a single struct iovec can overflow a ssize_t.
 */
#define	MAX_IOVEC	1024

typedef struct iovec
{
	void *	iov_base;
	size_t	iov_len;
} iovec_t;

/****************************************************************************/

extern ssize_t readv(int file_descriptor,const struct iovec *iov,int vec_count);
extern ssize_t writev(int file_descriptor,const struct iovec *iov,int vec_count);

__END_DECLS

#endif /* _SYS_UIO_H */
