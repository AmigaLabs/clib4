/*
 * $Id: math_signgam.c,v 2.0 2023-05-11 12:04:23 clib2devs Exp $
*/

#include <dos.h>

int *
__signgam(void) {
    struct _clib2 *__clib2 = __CLIB2;

    return &__clib2->_gamma_signgam;
}