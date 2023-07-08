/*
 * $Id: debug_kputs2.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

void
kputs(const UBYTE *s) {
    if (s != NULL)
        kputstr(s);
}
