/*
 * $Id: timezone_headers.h,v 1.0 2020-01-15 10:00:23 clib4devs Exp $
*/

#ifndef _TIMEZONE_HEADERS_H
#define _TIMEZONE_HEADERS_H

/****************************************************************************/

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

/****************************************************************************/

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

/****************************************************************************/

#include <locale.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

#include <time.h>

extern void __timezone_lock(void);
extern void __timezone_unlock(void);

#endif /* _TIMEZONE_HEADERS_H */
