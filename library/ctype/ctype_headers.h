/*
 * $Id: ctype_headers.h,v 1.6 2006-01-08 12:04:22 clib4devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#define _CTYPE_HEADERS_H

/****************************************************************************/

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_LOCALEBASE_H
#include "stdlib_localebase.h"
#endif /* _STDLIB_LOCALEBASE_H */

#ifndef _STDLIB_UTILITYBASE_H
#include "stdlib_utilitybase.h"
#endif /* _STDLIB_UTILITYBASE_H */

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

/****************************************************************************/

#include <locale.h>
#include <assert.h>
#include <limits.h>
#include <ctype.h>

/****************************************************************************/

extern int __isdigit_r(struct _clib4 *__clib4, int c);
extern int __isspace_r(struct _clib4 *__clib4, int c);

#endif /* _CTYPE_HEADERS_H */
