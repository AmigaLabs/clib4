/*
 * $Id: nl_types.h,v 1.0 2021-01-15 22:35:54 clib4devs Exp $
*/

#ifndef _NL_TYPES_H
#define _NL_TYPES_H

#include <features.h>

__BEGIN_DECLS

/**
 * catopen() flag to use the current locale.
 */
#define NL_CAT_LOCALE 1
/**
 * catgets() default set number.
 */
#define NL_SETD 1
/** Message catalog type. */
typedef void* nl_catd;
/** The type of the constants in `<langinfo.h>`, used by nl_langinfo(). */
typedef int nl_item;

/****************************************************************************/

extern nl_catd catopen(const char* name, int flag);
extern char* catgets(nl_catd catalog, int set_number, int msg_number, const char* msg);
extern int catclose(nl_catd catalog);

__END_DECLS

#endif /* _NL_TYPES_H */
