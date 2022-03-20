/*
 * $Id: stdlib_inistate.c,v 1.0 2022-02-23 12:23:12 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */


char *initstate(unsigned seed, char *state, size_t size) {
    void *old;

    if (size < 8)
        return 0;

    ObtainSemaphore(__global_clib2->__random_lock);

    old = savestate();
    if (size < 32)
        __global_clib2->n = 0;
    else if (size < 64)
        __global_clib2->n = 7;
    else if (size < 128)
        __global_clib2->n = 15;
    else if (size < 256)
        __global_clib2->n = 31;
    else
        __global_clib2->n = 63;
    __global_clib2->x = (uint32_t*)state + 1;
    __srandom(seed);
    savestate();

    ReleaseSemaphore(__global_clib2->__random_lock);

    return old;
}