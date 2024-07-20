/*
 * $Id: errno_data.c,v 1.4 2023-04-27 12:04:22 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int *
__errno() {
    return &__CLIB4->_errno;
}

int *
__errno_r(struct _clib4 *__clib4) {
    return &__clib4->_errno;
}

int *
__h_errno() {
    return &__CLIB4->_h_errno;
}

int *
__h_errno_r(struct _clib4 *__clib4) {
    return &__clib4->_h_errno;
}