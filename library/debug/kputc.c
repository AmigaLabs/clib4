/*
 * $Id: debug_kputc.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include "debug_headers.h"

VOID
kputc(UBYTE c) {
    /* Call RawPutChar() through the 68k LVO. */
    EmulateTags(SysBase, ET_Offset, -516, ET_RegisterD0, c, TAG_DONE);
}
