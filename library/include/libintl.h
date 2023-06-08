#ifndef _LIBINTL_H
#define _LIBINTL_H

#include <features.h>

__BEGIN_DECLS

#define __USE_GNU_GETTEXT 1
#define __GNU_GETTEXT_SUPPORTED_REVISION(major) ((major) == 0 ? 1 : -1)

extern char *gettext(const char *);
extern char *dgettext(const char *, const char *);
extern char *dcgettext(const char *, const char *, int);
extern char *ngettext(const char *, const char *, unsigned long);
extern char *dngettext(const char *, const char *, const char *, unsigned long);
extern char *dcngettext(const char *, const char *, const char *, unsigned long, int);
extern char *textdomain(const char *);
extern char *bindtextdomain (const char *, const char *);
extern char *bind_textdomain_codeset(const char *, const char *);

__END_DECLS

#endif
