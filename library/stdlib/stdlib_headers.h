/*
 * $Id: stdlib_headers.h,v 1.22 2010-10-20 13:50:17 clib4devs Exp $
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
#include "../include/dos.h"

#ifndef _STDLIB_LOCALEBASE_H
#include "stdlib_localebase.h"
#endif /* _STDLIB_LOCALEBASE_H */

#ifndef _STDLIB_UTILITYBASE_H
#include "stdlib_utilitybase.h"
#endif /* _STDLIB_UTILITYBASE_H */

#ifndef _STDLIB_TIMEZONEBASE_H
#include "stdlib_timezonebase.h"
#endif /* _STDLIB_TIMEZONEBASE_H */

#ifndef _STDLIB_TIMERBASE_H
#include "stdlib_timerbase.h"
#endif /* _STDLIB_TIMERBASE_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

#include "gdtoa.h"
#include "mprec.h"

#include <math.h>

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

extern unsigned int (* __get_default_stack_size)(void);

extern void _init(void);
extern void _fini(void);

extern int _main(char *argstr,
                 int arglen,
                 int (*start_main)(int, char **),
                 void (*__EXT_CTOR_LIST__[])(void),
                 void (*__EXT_DTOR_LIST__[])(void));

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

extern void kprintf(const char * format,...);

/* Old constructors and destructors are moved here */
void call_constructors(void);
void call_destructors(void);

void _stdlib_program_name_init(void);
void _stdlib_memory_init(void);
void _stdio_init(void);
void _stdio_file_init(void);
void _socket_init(void);
void _math_init(void);
void _arg_init(void);
void _usergroup_init(void);
void _locale_init(void);
void _unistd_init(void);
void _dirent_init(void);
void _timer_init(void);
void _timezone_init(void);
void _clock_init(void);

void _stdlib_program_name_exit(void);
void _stdlib_memory_exit(void);
void _stdio_exit(void);
void _stdio_file_exit(void);
void _socket_exit(void);
void _arg_exit(void);
void _locale_exit(void);
void _unistd_exit(void);
void _dirent_exit(void);
void _chdir_exit(void);
void _setenv_exit(void);
void _timezone_exit(void);
void _timer_exit(void);
void _wildcard_expand_exit(void);
void _usergroup_exit(void);
void _dcngettext_exit(void);

#endif /* _STDLIB_HEADERS_H */
