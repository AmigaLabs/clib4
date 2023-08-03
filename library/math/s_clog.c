/*
 * $Id: math_s_clog.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
clog(double complex z) {
    double complex w;
    double p, rr;

    /*rr = sqrt( z->r * z->r  +  z->i * z->i );*/
    rr = cabs(z);
    p = log(rr);
    rr = atan2(cimag(z), creal(z));
    w = p + rr * I;
    return (w);
}
