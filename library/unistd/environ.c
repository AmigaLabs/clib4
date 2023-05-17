/*
 * $Id: unistd_environ.c,v 2.0 2023-05-17 08:00:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char ***
__environ(void) {
    struct _clib2 *__clib2 = __CLIB2;
    return &__clib2->__environment;
}