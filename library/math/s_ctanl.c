/*
 * $Id: math_s_ctanl.c,v 1.0 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
ctanl(long double complex z) {
    return ctan(z);
}
