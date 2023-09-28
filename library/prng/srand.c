/*
 * $Id: stdlib_srand.c,v 1.4 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
srand(unsigned seed)  {
    ENTER();
    SHOWVALUE(seed);
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->__random_seed = seed - 1;

    LEAVE();
}
