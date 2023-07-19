/*
 * $Id: math_s_cargl.c,v 1.2 2023-07-19 12:04:22 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
cargl(long double complex z) {
    return atan2l(cimagl(z), creall(z));
}
