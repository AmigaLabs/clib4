/*
 * $Id: stddef.h,v 1.7 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _STDDEF_H
#define _STDDEF_H

#include <features.h>
#include <limits.h>

__BEGIN_DECLS

#ifndef NULL
#ifndef __cplusplus
#define NULL ((void *)0L)
#else
#define NULL 0L
#endif /* __cplusplus */
#endif /* NULL */

typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef struct { long long __ll; long double __ld; } max_align_t;

/* wchar_t is a built-in type in C++ */
#ifndef __cplusplus
typedef int wchar_t;
#endif

#define offsetof(type, member) ((size_t)&((type *)0)->member)

__END_DECLS

#endif /* _STDDEF_H */
