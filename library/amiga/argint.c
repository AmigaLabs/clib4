/*
 * $Id: amiga_argint.c,v 1.3 2006-01-08 12:04:22 clib4devs Exp $
*/

#include <workbench/workbench.h>
#include <exec/memory.h>

#include <string.h>

#include <proto/dos.h>
#include <proto/icon.h>
#include <clib/alib_protos.h>

#include "debug.h"

LONG
ArgInt(CONST_STRPTR *tt, CONST_STRPTR entry, LONG default_val) {
    STRPTR str;

    if (tt != NULL && IconBase != NULL && ((str = (STRPTR) FindToolType((STRPTR *) tt, (STRPTR) entry))) != NULL)
        StrToLong(str, &default_val);

    return (default_val);
}
