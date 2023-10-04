/*
 * $Id: unistd_environ.c,v 2.0 2023-05-17 08:00:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char ***
__environ(void) {
    struct _clib4 *__clib4 = __CLIB4;
    return &__clib4->__environment;
}