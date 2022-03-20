/*
 * $Id: math_truncf.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float truncf(float x)
{
  LONG signbit, w, exponent_less_127;

  GET_FLOAT_WORD(w, x);

  /* Extract sign bit. */
  signbit = w & 0x80000000;

  /* Extract exponent field. */
  exponent_less_127 = ((w & 0x7f800000) >> 23) - 127;

  if (exponent_less_127 < 23)
  {
    if (exponent_less_127 < 0)
    {
      /* -1 < x < 1, so result is +0 or -0. */
      SET_FLOAT_WORD(x, signbit);
    }
    else
    {
      SET_FLOAT_WORD(x, signbit | (w & ~(0x007fffff >> exponent_less_127)));
    }
  }
  else
  {
    if (exponent_less_127 == 255)
      /* x is NaN or infinite. */
      return x + x;

    /* All bits in the fraction field are relevant. */
  }
  return x;
}
