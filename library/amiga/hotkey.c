/*
 * $Id: amiga_hotkey.c,v 1.3 2006-01-08 12:04:22 clib4devs Exp $
*/

#include <libraries/commodities.h>

#include <string.h>

#include <proto/commodities.h>
#include <clib/alib_protos.h>

#include "debug.h"

CxObj *
HotKey(CONST_STRPTR descr, struct MsgPort *port, LONG ID) {
    CxObj *result = NULL;
    CxObj *filter = NULL;

    assert(descr != NULL && port != NULL);

    if (CxBase == NULL)
        goto out;

    filter = CxFilter(descr);
    if (filter == NULL)
        goto out;

    AttachCxObj(filter, CxSender(port, ID));
    AttachCxObj(filter, CxTranslate(NULL));

    if (CxObjError(filter))
        goto out;

    result = filter;

out:

    if (result == NULL && filter != NULL)
        DeleteCxObjAll(filter);

    return (result);
}
