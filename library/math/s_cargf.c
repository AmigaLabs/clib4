/*
 * $Id: math_s_cargf.c,v 1.2 2023-07-19 12:04:22 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
cargf(float complex z) {
    return atan2f(cimagf(z), crealf(z));
}
