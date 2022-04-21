/*
 * $Id: debug_headers.h,v 1.4 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _DEBUG_HEADERS_H
#define _DEBUG_HEADERS_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef EXEC_EMULATION_H
#include <exec/emulation.h>
#endif /* EXEC_EMULATION_H */

#ifndef __amigaos4__
#define SysBase (*(struct Library **)4)
#define IExec ((struct ExecIFace *)((struct ExecBase *)SysBase)->MainInterface)
#endif

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

/****************************************************************************/

#include <string.h>
#include <stdarg.h>

/****************************************************************************/

extern LONG cmpstrexec(const char *source1, const char *source2);
extern LONG KCmpStr(const char *source1, const char *source2);
extern LONG KMayGetCh(VOID);
extern LONG KMayGetChar(VOID);
extern LONG kgetc(VOID);
extern LONG kgetchar(VOID);
extern LONG kgetch(VOID);
extern LONG KGetCh(VOID);
extern LONG KGetChar(VOID);
extern LONG kgetnum(VOID);
extern LONG KGetNum(void);
extern VOID KDoFmt(const char *format_string, APTR data_stream, APTR put_char_routine, APTR put_char_data);
extern VOID KPutFmt(const char *format, va_list arg);
extern VOID KVPrintF(const char * format, va_list arg);
extern VOID kvprintf(const char *format, va_list arg);
extern VOID kprintf(const char *format, ...);
extern VOID KPrintF(const char *format, ...);
extern VOID kputc(UBYTE c);
extern VOID kputchar(UBYTE c);
extern VOID kputch(UBYTE c);
extern VOID KPutCh(UBYTE c);
extern VOID KPutChar(UBYTE c);
extern VOID kputstr(const UBYTE *s);
extern VOID KPutS(const UBYTE *s);
extern VOID kputs(const UBYTE *s);
extern VOID KPutStr(const UBYTE *s);

#endif /* _DEBUG_HEADERS_H */
