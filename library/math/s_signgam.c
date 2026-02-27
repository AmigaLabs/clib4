/*
 * $Id: math_s_signgam.c,v 2.0 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#include <dos.h>

int *
__signgam(void) {
    struct _clib4 *__clib4 = __CLIB4;

    return &__clib4->_gamma_signgam;
}