/*
 * $Id: strings.h,v 1.8 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _STRINGS_H
#define _STRINGS_H

#ifndef _STDDEF_H
#include <stddef.h>
#endif /* _STDDEF_H */

#include <features.h>

__BEGIN_DECLS

/* These come from 4.4BSD. */
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t len);

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE)  || defined(_BSD_SOURCE)
extern int ffs(int i);
extern int ffsl(long i);
extern int ffsll(long long i);
#endif

__END_DECLS

#endif /* _STRINGS_H */
