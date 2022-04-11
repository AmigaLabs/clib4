/*
 * $Id: time_srdtsc.c,v 1.0 2022-04-09 18:06:24 clib2devs Exp $
 *
 */

#include <time.h>
#include <inttypes.h>

// This is a generic GCC version for 32 bit PowerPC machines.
// We need to loop because we are reading two registers and the high one might change.
uint64_t rdtsc(void)
{
    uint32_t high1 = 0, high2 = 0, low = 0;

    __asm__ __volatile__
    (
    "1: mftbu %0\n"
    "mftb  %1\n"
    "mftbu %2\n"
    "cmpw  %3,%4\n"
    "bne-  1b\n"
    : "=r" (high1), "=r" (low), "=r" (high2)
    : "0" (high1), "2" (high2)
    );

    return ((uint64_t)(high1) << 32) | low;
}