/*
 * $Id: aio_aio_init.c,v 1.0 2023-03-03 07:15:37 clib2devs Exp $
*/
#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <aio.h>
#include "aio_misc.h"

#ifdef __USE_GNU
void aio_init(const struct aioinit *init) {
    return __aio_init(init);
}
#endif