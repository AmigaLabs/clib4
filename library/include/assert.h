/*
 * $Id: assert.h,v 1.6 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _ASSERT_H
#define _ASSERT_H

#include <features.h>

__BEGIN_DECLS

#ifndef assert
#ifndef NDEBUG

extern void __assertion_failure(const char *file_name, int line_number, const char *expression);

#define assert(expression) \
    ((void)((expression) ? 0 : (__assertion_failure(__FILE__,__LINE__,#expression),0)))

#else
inline void __assertion_failure(const char *file_name, int line_number, const char *expression) {};
#define assert(expression) ((void)0)
#endif /* NDEBUG */
#endif /* assert */

__END_DECLS

#endif /* _ASSERT_H */
