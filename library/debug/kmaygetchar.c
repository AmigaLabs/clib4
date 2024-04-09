/*
 * $Id: debug_kmaygetchar.c,v 1.4 2024-04-09 12:04:22 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "debug_headers.h"

LONG
KMayGetChar(VOID) {
    /* Call RawMayGetChar() through the 68k LVO. */
    return EmulateTags(SysBase, ET_Offset, -510, TAG_DONE);
}
