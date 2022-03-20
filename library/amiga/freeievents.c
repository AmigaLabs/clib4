/*
 * $Id: amiga_freeievents.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include <devices/inputevent.h>

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

VOID
FreeIEvents(struct InputEvent *ie) {
    struct InputEvent *next;

    while (ie != NULL) {
        next = ie->ie_NextEvent;

        FreeVec(ie);

        ie = next;
    }
}
