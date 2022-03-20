/*
 * $Id: amiga_argarraydone.c,v 1.4 2006-09-25 14:05:31 clib2devs Exp $
*/

#include <workbench/workbench.h>
#include <exec/memory.h>

#include <string.h>

#include <proto/exec.h>
#include <proto/icon.h>
#include <clib/alib_protos.h>

#include "debug.h"

extern const unsigned char **CXLIB_argarray;
extern struct DiskObject *CXLIB_disko;

VOID
ArgArrayDone(VOID) {
    if (CXLIB_argarray != NULL) {
        FreeVec(CXLIB_argarray);
        CXLIB_argarray = NULL;
    }

    if (CXLIB_disko != NULL) {
        FreeDiskObject(CXLIB_disko);
        CXLIB_disko = NULL;
    }
}
