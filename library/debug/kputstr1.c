/*
 * $Id: debug_kputstr1.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

void
kputstr(const UBYTE *s) {
    assert(s != NULL);

    if (s != NULL) {
        UBYTE c;

        while ((c = (*s++)) != '\0')
            kputc(c);
    }
}
