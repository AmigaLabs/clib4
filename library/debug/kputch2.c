/*
 * $Id: debug_kputch2.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

VOID
KPutCh(UBYTE c) {
    kputc(c);
}
