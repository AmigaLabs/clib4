/*
 * $Id: amiga_argstring.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include <libraries/commodities.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <exec/memory.h>

#include <string.h>

#include <proto/icon.h>
#include <clib/alib_protos.h>

#include "debug.h"

STRPTR
ArgString(CONST_STRPTR *tt, CONST_STRPTR entry, CONST_STRPTR default_string) {
    STRPTR result;

    if (tt != NULL && IconBase != NULL && ((result = (STRPTR) FindToolType((STRPTR *) tt, (STRPTR) entry))) != NULL)
        return (result);
    else
        return ((STRPTR) default_string);
}
