/*
 * $Id: stdlib_srand48.c,v 1.0 2022-08-06 10:36:26 clib4devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void srand48(long seed) {
    seed48((unsigned short[3]) {0x330e, seed, seed >> 16});
}
