/*
 * $Id: stdlib_rand.c,v 1.7 2022-09-01 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
rand(void) {
    struct _clib4 *__clib4 = __CLIB4;

    __clib4->__random_seed = 6364136223846793005ULL * __clib4->__random_seed + 1;

    return (__clib4->__random_seed >> 33);
}
