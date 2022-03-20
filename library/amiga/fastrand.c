/*
 * $Id: amiga_fastrand.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include <exec/types.h>
#include <clib/alib_protos.h>

ULONG
FastRand(ULONG seed) {
    ULONG value;

    value = seed + seed;
    if (value < seed)
        value ^= 0x1D872B41;

    return (value);
}
