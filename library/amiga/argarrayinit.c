/*
 * $Id: amiga_argarrayinit.c,v 1.7 2021-01-31 14:09:00 clib2devs Exp $
*/

#include <libraries/commodities.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <exec/memory.h>

#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/commodities.h>
#include <clib/alib_protos.h>

#include "debug.h"

const unsigned char **CXLIB_argarray;
struct DiskObject *CXLIB_disko;


STRPTR *
ArgArrayInit(LONG argc, CONST_STRPTR *argv) {
    STRPTR *result = NULL;

    if (argc != 0) /* run from CLI */
    {
        LONG i;

        if (argc == 1)
            goto out; /* skip command name */

        CXLIB_argarray = (const unsigned char **) AllocVecTags(sizeof(char *) * argc, AVT_Type, MEMF_SHARED,
                                                               AVT_ClearWithValue, 0, TAG_DONE);
        if (CXLIB_argarray == NULL)
            goto out;

        for (i = 1; i < argc; i++)
            CXLIB_argarray[i - 1] = (unsigned char *) argv[i];

        result = (STRPTR *) CXLIB_argarray;
    } else if (IconBase != NULL) {
        struct WBStartup *msg;

        /* run from WB */
        msg = (struct WBStartup *) argv;

        CXLIB_disko = GetDiskObject(msg->sm_ArgList[0].wa_Name);
        if (CXLIB_disko == NULL)
            goto out;

        result = (STRPTR *) CXLIB_disko->do_ToolTypes;
    }

out:

    return (result);
}
