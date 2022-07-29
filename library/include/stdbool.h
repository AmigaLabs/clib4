/*
 * $Id: stdbool.h,v 1.6 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _STDBOOL_H
#define _STDBOOL_H

#if defined(__cplusplus) && defined(__GNUC__)
#warning C99 header used in C++.
#endif /* __GNUC__ */

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
#warning C99 header file used by non-C99 compliant compiler.
#endif /* !__STDC_VERSION__ || __STDC_VERSION__ < 199901L */

#include <features.h>

__BEGIN_DECLS

#define bool _Bool

#define false	0
#define true	1

#define __bool_true_false_are_defined 1

__END_DECLS

#endif /* _STDBOOL_H */
