/*
 * $Id: debug_kprintf2.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

VOID
KPrintF(const char *format, ...) {
    assert(format != NULL);

    if (format != NULL) {
        va_list arg;

        va_start(arg, format);
        KPutFmt(format, arg);
        va_end(arg);
    }
}
