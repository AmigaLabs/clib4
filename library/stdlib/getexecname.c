/*
* $Id: stdlib_getexecname.c,v 1.1 2025-08-16 13:28:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

const char *
getexecname(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    RETURN(__clib4->__progname);
    return strdup(__clib4->__progname);
}
