/*
 * $Id: stdlib_srand.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
srand(unsigned seed)  {
    ENTER();
    SHOWVALUE(seed);
    struct _clib2 *__clib2 = __CLIB2;

    __clib2->__random_seed = seed - 1;

    LEAVE();
}
