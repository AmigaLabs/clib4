/*
 * $Id: stdlib_setstate.c,v 1.0 2022-02-23 12:23:12 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
setstate(const char *state) {
    char *old;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();
    SHOWSTRING(state);

    ObtainSemaphore(__clib2->__random_lock);

    old = savestate();
    loadstate((uint32_t*)state);

    ReleaseSemaphore(__clib2->__random_lock);

    RETURN(old);
    return old;
}