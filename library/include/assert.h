/*
 * $Id: assert.h,v 1.7 2023-06-03 12:06:14 clib2devs Exp $
*/

#ifndef _ASSERT_H
#define _ASSERT_H

#include <features.h>

__BEGIN_DECLS

extern void __assertion_failure(const char *file_name, int line_number, const char *expression);

#ifndef NDEBUG
    #define assert(expression) ((void)((expression) ? 0 : (__assertion_failure(__FILE__,__LINE__,#expression),0)))
#else
    #define assert(expression) ((void)0)
#endif /* NDEBUG */

__END_DECLS

#endif /* _ASSERT_H */
