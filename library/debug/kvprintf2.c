/*
 * $Id: debug_kvprintf2.c,v 1.3 2006-01-08 12:04:22 clib4devs Exp $
*/

#include "debug_headers.h"

VOID
KVPrintF(const char *format, va_list arg) {
    if (format != NULL)
        KPutFmt(format, arg);
}
