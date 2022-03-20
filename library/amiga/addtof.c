/*
 * $Id: amiga_addtof.c,v 1.4 2006-01-08 12:04:22 clib2devs Exp $
 */

#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <graphics/graphint.h>

#include <string.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <clib/alib_protos.h>

#include "macros.h"

#include "debug.h"

typedef LONG (*CFUNC)(APTR arg);

STATIC LONG
INTERRUPT ASM
call_routine (REG(a1, struct Isrvstr *i)) {
    CFUNC p = (CFUNC) i->ccode;

    (*p)(i->Carg);

    return (0);
}

VOID
AddTOF(struct Isrvstr *i, CFUNC p, APTR arg) {
    assert(i != NULL && p != NULL);

    i->code = (long (*)()) call_routine;
    i->Iptr = i;
    i->ccode = p;
    i->Carg = arg;

    AddIntServer(INTB_VERTB, (struct Interrupt *) i);
}
