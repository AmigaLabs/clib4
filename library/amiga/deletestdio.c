/*
 * $Id: amiga_deletestdio.c,v 1.6 2021-01-31 15:12:47 clib2devs Exp $
*/

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

#if defined(DeleteStdIO)
#undef DeleteStdIO
#endif /* DeleteStdIO */

VOID DeleteStdIO(struct IOStdReq *io);

VOID DeleteStdIO(struct IOStdReq *io) {
    FreeSysObject(ASOT_IOREQUEST, io);
}
