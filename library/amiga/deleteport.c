/*
 * $Id: amiga_deleteport.c,v 1.6 2021-01-31 15:12:47 clib4devs Exp $
*/

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

#if defined(DeletePort)
#undef DeletePort
#endif /* DeletePort */

VOID DeletePort(struct MsgPort *port);

VOID
DeletePort(struct MsgPort *port) {
    assert(port != NULL);

    if (port->mp_Node.ln_Name != NULL)
        RemPort(port);

    FreeSysObject(ASOT_PORT, port);
}
