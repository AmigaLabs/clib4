/*
 * $Id: amiga_beginio.c,v 1.5 2006-09-25 15:12:47 clib4devs Exp $
*/

#include <exec/devices.h>
#include <exec/io.h>
#include <clib/alib_protos.h>

#include "debug.h"

#include <proto/exec.h>

#if defined(BeginIO)
#undef BeginIO
#endif /* BeginIO */

VOID BeginIO(struct IORequest *ior);

VOID
BeginIO(struct IORequest *ior) {
    ENTER();

    assert(ior != NULL && ior->io_Device != NULL);

    IExec->BeginIO(ior);

    LEAVE();
}
