/*
 * $Id: amiga_waitbeam.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <graphics/graphint.h>

#include <string.h>

#include <proto/graphics.h>
#include <clib/alib_protos.h>

#include "debug.h"

#define DO_NOTHING ((void)0)

VOID
waitbeam(LONG position) {
    while (position > VBeamPos())
        DO_NOTHING;
}
