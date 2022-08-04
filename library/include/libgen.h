/*
 * $Id: libgen.h,v 1.7 2006-09-25 13:29:47 clib2devs Exp $
*/

#ifndef _LIBGEN_H
#define _LIBGEN_H

#include <features.h>

__BEGIN_DECLS

extern char *basename(const char *path);
extern char *dirname(const char *path);

__END_DECLS

#endif /* _LIBGEN_H */
