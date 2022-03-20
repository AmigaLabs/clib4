/*
 * $Id: clib2_stdc.h,v 1.4 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef	_SYS_CLIB2_STDC_H
#define	_SYS_CLIB2_STDC_H

#include <features.h>

__BEGIN_DECLS

/* Make the "restrict" qualifier work well with 'C' compilers that do
   not support it. We do assume that __STDC__ is defined, though. */
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
#define restrict
#endif /* !__STDC_VERSION__ || __STDC_VERSION__ < 199901L */

__END_DECLS

#endif /* _SYS_CLIB2_STDC_H */
