/*
 * $Id: amiga_createport.c,v 1.6 2021-01-31 15:12:47 clib2devs Exp $
*/

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

#if defined(CreatePort)
#undef CreatePort
#endif /* CreatePort */

struct MsgPort *CreatePort(CONST_STRPTR name, LONG pri);

struct MsgPort *
CreatePort(CONST_STRPTR name, LONG pri) {
    struct MsgPort *result = NULL;

    assert(-128 <= pri && pri <= 127);

    if (pri < -128 || pri > 127)
        goto out;

    result = AllocSysObjectTags(ASOT_PORT, ASOPORT_AllocSig, FALSE, ASOPORT_Signal, SIGB_SINGLE, TAG_DONE);
    if (result != NULL && name != NULL) {
        result->mp_Node.ln_Name = (char *) name;
        result->mp_Node.ln_Pri = pri;

        AddPort(result);
    }

out:

    return (result);
}
