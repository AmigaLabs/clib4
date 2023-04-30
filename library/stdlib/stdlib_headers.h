/*
 * $Id: stdlib_headers.h,v 1.22 2010-10-20 13:50:17 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#define _STDLIB_HEADERS_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef DOS_ANCHORPATH_H
#include <dos/anchorpath.h>
#endif /* DOS_ANCHORPATH_H */

#ifndef DOS_DOSEXTENS_H
#include <dos/dosextens.h>
#endif /* DOS_DOSEXTENS_H */

#ifndef DOS_DOSTAGS_H
#include <dos/dostags.h>
#endif /* DOS_DOSTAGS_H */

#ifndef DOS_DOSASL_H
#include <dos/dosasl.h>
#endif /* DOS_DOSASL_H */

#include <dos/obsolete.h>

#ifndef WORKBENCH_STARTUP_H
#include <workbench/startup.h>
#endif /* WORKBENCH_STARTUP_H */

#ifndef CLIB_ALIB_PROTOS_H
#include <clib/alib_protos.h>
#endif /* CLIB_ALIB_PROTOS_H */

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

#ifndef PROTO_DOS_H
#include <proto/dos.h>
#endif /* PROTO_DOS_H */

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <endian.h>
#include <dos.h>

#ifndef _STDLIB_LOCALEBASE_H
#include "stdlib_localebase.h"
#endif /* _STDLIB_LOCALEBASE_H */

#ifndef _STDLIB_UTILITYBASE_H
#include "stdlib_utilitybase.h"
#endif /* _STDLIB_UTILITYBASE_H */

#ifndef _STDLIB_TIMEZONEBASE_H
#include "stdlib_timezonebase.h"
#endif /* _STDLIB_TIMEZONEBASE_H */

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

#include <math.h>

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

#define MIN_OS_VERSION 52

/* Memalign memory list */
extern void           *__memalign_pool;
extern struct AVLNode *__memalign_tree;

extern jmp_buf NOCOMMON __exit_jmp_buf;
extern int NOCOMMON		__exit_value;

extern unsigned int NOCOMMON (* __get_default_stack_size)(void);
extern unsigned int NOCOMMON __stack_size;

extern BOOL NOCOMMON __is_resident;
extern UBYTE NOCOMMON __shell_escape_character;

extern char **__argv;
extern int __argc;

extern BOOL NOCOMMON __lib_startup;

extern void _init(void);
extern void _fini(void);

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

extern void kprintf(const char * format,...);

#endif /* _STDLIB_HEADERS_H */
