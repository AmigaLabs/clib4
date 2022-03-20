/*
 * $Id: math_trunc.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
trunc(double x)
{
  int signbit;
  /* Most significant word, least significant word. */
  int msw;
  unsigned int lsw;
  int exponent_less_1023;

  EXTRACT_WORDS(msw, lsw, x);

  /* Extract sign bit. */
  signbit = msw & 0x80000000;

  /* Extract exponent field. */
  exponent_less_1023 = ((msw & 0x7ff00000) >> 20) - 1023;

  if (exponent_less_1023 < 20)
  {
    /* All significant digits are in msw. */
    if (exponent_less_1023 < 0)
    {
      /* -1 < x < 1, so result is +0 or -0. */
      INSERT_WORDS(x, signbit, 0);
    }
    else
    {
      /* All relevant fraction bits are in msw, so lsw of the result is 0. */
      INSERT_WORDS(x, signbit | (msw & ~(0x000fffff >> exponent_less_1023)), 0);
    }
  }
  else if (exponent_less_1023 > 51)
  {
    if (exponent_less_1023 == 1024)
    {
      /* x is infinite, or not a number, so trigger an exception. */
      return x + x;
    }
    /* All bits in the fraction fields of the msw and lsw are needed in the result. */
  }
  else
  {
    /* All fraction bits in msw are relevant.  Truncate irrelevant
         bits from lsw. */
    INSERT_WORDS(x, msw, lsw & ~(0xffffffffu >> (exponent_less_1023 - 20)));
  }
  return x;
}
