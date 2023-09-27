/*
 * $Id: amiga_remtof.c,v 1.3 2006-01-08 12:04:22 clib4devs Exp $
*/

#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <graphics/graphint.h>

#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include "debug.h"

VOID
RemTOF(struct Isrvstr *i) {
    assert(i != NULL);

    RemIntServer(INTB_VERTB, (struct Interrupt *) i);
}
