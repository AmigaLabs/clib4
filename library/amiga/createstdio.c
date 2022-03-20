/*
 * $Id: amiga_createstdio.c,v 1.6 2021-09-31 15:12:47 clib2devs Exp $
*/

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

#if defined(CreateStdIO)
#undef CreateStdIO
#endif /* CreateStdIO */

struct IOStdReq *CreateStdIO(CONST struct MsgPort *port);

struct IOStdReq *
CreateStdIO(CONST struct MsgPort *port) {
    struct IOStdReq *result = NULL;

    assert(port != NULL);

    if (port == NULL)
        goto out;

    result = AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_Size, sizeof(struct IOStdReq), ASOIOR_ReplyPort, port, TAG_END);

out:

    return (result);
}
