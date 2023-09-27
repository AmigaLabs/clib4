/*
 * $Id: amiga_rangerand.c,v 1.3 2006-01-08 12:04:22 clib4devs Exp $
*/

#include <exec/types.h>

#include <clib/alib_protos.h>

ULONG RangeSeed;

UWORD
RangeRand(ULONG max_value) {
    ULONG next_value;
    ULONG result;
    ULONG value;
    UWORD range;
    UWORD bits;

    range = (max_value & 0xFFFF);
    bits = range - 1;

    value = RangeSeed;

    do {
        next_value = value + value;
        if (next_value <= value)
            next_value ^= 0x1D872B41;

        value = next_value;

        bits = bits >> 1;
    } while (bits != 0);

    RangeSeed = value;

    if (range == 0)
        result = value & 0xFFFF;
    else
        result = (range * (value & 0xFFFF)) >> 16;

    return ((UWORD) result);
}
