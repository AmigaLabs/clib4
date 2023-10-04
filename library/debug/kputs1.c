/*
 * $Id: debug_kputs1.c,v 1.3 2006-01-08 12:04:22 clib4devs Exp $
*/

#include "debug_headers.h"

void
KPutS(const UBYTE *s) {
    if (s != NULL)
        kputstr(s);
}
