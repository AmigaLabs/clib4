/*
 * $Id: stdlib_setstate.c,v 1.0 2022-02-23 12:23:12 clib4devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
setstate(const char *state) {
    char *old;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWSTRING(state);

    ObtainSemaphore(__clib4->__random_lock);

    old = savestate();
    loadstate((uint32_t*)state);

    ReleaseSemaphore(__clib4->__random_lock);

    RETURN(old);
    return old;
}