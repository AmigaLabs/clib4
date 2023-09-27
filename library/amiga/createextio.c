/*
 * $Id: amiga_createextio.c,v 1.6 2021-01-31 15:12:47 clib4devs Exp $
*/

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

#if defined(CreateExtIO)
#undef CreateExtIO
#endif /* CreateExtIO */

struct IORequest *CreateExtIO(CONST struct MsgPort *port, LONG io_size);

struct IORequest *
CreateExtIO(CONST struct MsgPort *port, LONG io_size) {
    struct IORequest *result = NULL;

    assert(port != NULL && io_size > 0);

    if (port == NULL || io_size < (LONG)sizeof(*result))
    goto out;
    result = AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_Size, sizeof(struct IORequest), ASOIOR_ReplyPort, port, TAG_END);

out:

    return (result);
}
