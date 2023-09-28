/*
 * $Id: locale_headers.h,v 2.0 2023-05-03 12:04:23 clib4devs Exp $
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

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

#include <locale.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <langinfo.h>

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

extern void __locale_lock(struct _clib4 *__clib4);
extern void __locale_unlock(struct _clib4 *__clib4);
extern void __close_all_locales(struct _clib4 *__clib4);

#endif /* _LOCALE_HEADERS_H */
