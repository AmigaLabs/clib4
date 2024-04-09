/*
 * $Id: debug_kputc.c,v 1.4 2024-04-09 12:04:22 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "debug_headers.h"

VOID
kputc(UBYTE c) {
    /* Call RawPutChar() through the 68k LVO. */
    EmulateTags(SysBase, ET_Offset, -516, ET_RegisterD0, c, TAG_DONE);
}
