/*
 * $Id: amiga_freeievents.c,v 1.3 2006-01-08 12:04:22 clib4devs Exp $
*/

#include <devices/inputevent.h>

#include <string.h>

#include <clib/alib_protos.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "debug.h"

VOID
FreeIEvents(struct InputEvent *ie) {
    struct InputEvent *next;

    while (ie != NULL) {
        next = ie->ie_NextEvent;

        free(ie);

        ie = next;
    }
}
