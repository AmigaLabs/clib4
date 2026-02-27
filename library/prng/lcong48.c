/*
 * $Id: stdlib_lcong48.c,v 1.0 2022-08-06 10:36:26 clib4devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "../stdlib/stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "rand48.h"

void lcong48(unsigned short p[7]) {
    memcpy(__seed48, p, sizeof __seed48);
}
