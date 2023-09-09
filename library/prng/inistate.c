/*
 * $Id: stdlib_inistate.c,v 1.0 2022-02-23 12:23:12 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
initstate(unsigned seed, char *state, size_t size) {
    struct _clib2 *__clib2 = __CLIB2;
    void *old;

    ENTER();
    SHOWVALUE(seed);
    SHOWPOINTER(state);
    SHOWVALUE(size);

    if (size < 8)
        return 0;

    ObtainSemaphore(__clib2->__random_lock);

    old = savestate();
    if (size < 32)
        __clib2->n = 0;
    else if (size < 64)
        __clib2->n = 7;
    else if (size < 128)
        __clib2->n = 15;
    else if (size < 256)
        __clib2->n = 31;
    else
        __clib2->n = 63;
    __clib2->x = (uint32_t*)state + 1;
    __srandom(seed);
    savestate();

    ReleaseSemaphore(__clib2->__random_lock);

    RETURN(old);
    return old;
}