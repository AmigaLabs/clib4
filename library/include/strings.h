/*
 * $Id: strings.h,v 1.8 2006-01-08 12:06:14 clib2devs Exp $
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
extern int ffs(int i);

/****************************************************************************/

/*
 * These two provide functions which are available with the Lattice and
 * SAS/C compiler runtime libraries. Which probably makes them more exotic
 * than XENIX.
 */
#define stricmp(s1, s2)			strcasecmp((s1), (s2))
#define strnicmp(s1, s2, len)	strncasecmp((s1), (s2), (len))

__END_DECLS

#endif /* _STRINGS_H */
