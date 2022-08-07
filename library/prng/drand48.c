/*
 * $Id: stdlib_drand48.c,v 1.0 2022-08-06 10:36:26 clib2devs Exp $
*/
#define _XOPEN_SOURCE

#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

double
drand48(void) {
    return erand48(__seed48);
}
