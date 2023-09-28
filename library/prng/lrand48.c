/*
 * $Id: stdlib_lrand48.c,v 1.0 2022-08-06 10:36:26 clib4devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

long lrand48(void) {
    return nrand48(__seed48);
}