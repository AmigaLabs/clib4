/*
 * $Id: stdlib_mrand48.c,v 1.0 2022-08-06 10:36:26 clib4devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

long
mrand48(void) {
    return jrand48(__seed48);
}
