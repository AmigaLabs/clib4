/*
 * $Id: amiga_deleteextio.c,v 1.6 2021-01-31 15:12:47 clib2devs Exp $
*/

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

#if defined(DeleteExtIO)
#undef DeleteExtIO
#endif /* DeleteExtIO */

VOID DeleteExtIO(struct IORequest *io);

VOID DeleteExtIO(struct IORequest *io) {
    assert(io != NULL);

    if (io != NULL)
        FreeSysObject(ASOT_IOREQUEST, io);
}
