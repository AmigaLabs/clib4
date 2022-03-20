/*
 * $Id: debug_kmaygetchar.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include "debug_headers.h"

LONG
KMayGetChar(VOID)
{
	LONG result;

    /* Call RawMayGetChar() through the 68k LVO. */
    result = EmulateTags(SysBase, ET_Offset, -510, TAG_DONE);

	return(result);
}
