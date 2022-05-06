/*
 * $Id: locale_headers.h,v 1.8 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _LOCALE_HEADERS_H
#define _LOCALE_HEADERS_H

/****************************************************************************/

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef LIBRARIES_LOCALE_H
#include <libraries/locale.h>
#endif /* LIBRARIES_LOCALE_H */

/****************************************************************************/

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

/****************************************************************************/

#include <locale.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

#include <time.h>

/****************************************************************************/

/* Category name handling variables.  */
#define NUM_LOCALES			(LC_MAX + 1)
#define MAX_LOCALE_NAME_LEN	256

extern char __lc_ctype[__LC_LAST];

/****************************************************************************/

extern struct Locale * NOCOMMON __default_locale;
extern struct Locale * NOCOMMON __locale_table[NUM_LOCALES];

/****************************************************************************/

extern char NOCOMMON __locale_name_table[NUM_LOCALES][MAX_LOCALE_NAME_LEN];

extern void __locale_lock(void);
extern void __locale_unlock(void);
extern void __close_all_locales(void);

/****************************************************************************/

#endif /* _LOCALE_HEADERS_H */
