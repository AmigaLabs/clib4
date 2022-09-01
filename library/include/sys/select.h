/*
 * $Id: select.h,v 1.9 2006-04-10 15:08:11 clib2devs Exp $
*/

#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#ifndef _STRING_H
#include <string.h>
#endif /* _STRING_H */

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif /* _SYS_TIME_H */

#include <features.h>

#include <signal.h>

__BEGIN_DECLS

/*
 * In case the select() data structures and macros are already
 * defined by somebody else...
 */
#ifndef FD_SET

/****************************************************************************/

/*
 * select() uses bit masks of file descriptors in longs. These macros
 * manipulate such bit fields.
 *
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE 256
#endif

typedef	struct fd_set
{
	unsigned long bits[(FD_SETSIZE + 31) / 32];
} fd_set;

#define	FD_SET(n,p)		((void)(((unsigned long)n) < FD_SETSIZE ? (p)->bits[((unsigned long)n) >> 5] |=  (1UL << (((unsigned long)n) & 31)) : 0))
#define	FD_CLR(n,p)		((void)(((unsigned long)n) < FD_SETSIZE ? (p)->bits[((unsigned long)n) >> 5] &= ~(1UL << (((unsigned long)n) & 31)) : 0))
#define	FD_ISSET(n,p)	(((unsigned long)n) < FD_SETSIZE && ((p)->bits[((unsigned long)n) >> 5] & (1UL << (((unsigned long)n) & 31))) != 0)
#define	FD_COPY(f,t)	((void)memmove(t,f,sizeof(*(f))))
#define	FD_ZERO(p)		((void)memset(p,0,sizeof(*(p))))

/****************************************************************************/

#endif /* FD_SET */

/****************************************************************************/

extern int select(int nfds, fd_set *readfds,fd_set *writefds, fd_set *errorfds,struct timeval *timeout);

/* This is a special select() function which takes an extra Amiga signal
   bit mask pointer parameter. This function works like select(), but it will
   also return if any of the signals indicated by the 'signal_mask' parameter
   are set. When this function returns, the variable pointed to by the
   'signal_mask' parameter will have all the bits set which were set at the
   time the function was called and for which signals arrived while the
   function was still running. When this function returns, any signals
   received while it was running for which bits were set in the 'signal_mask'
   parameter value will be cleared with the exception of SIGBREAKF_CTRL_C.
   In brief, wait_select() works exactly like the bsdsocket.library/WaitSelect()
   function. */

extern int waitselect(int num_fds,fd_set *read_fds,fd_set *write_fds,fd_set *except_fds,struct timeval *timeout,unsigned long * signal_mask);

__END_DECLS

#endif /* _SYS_SELECT_H */
