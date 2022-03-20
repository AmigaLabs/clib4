/*
 * $Id: math_round.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
round(double x)
{
  /* Most significant word, least significant word. */
  LONG msw, exponent_less_1023;
  ULONG lsw;

  EXTRACT_WORDS(msw, lsw, x);

  /* Extract exponent field. */
  exponent_less_1023 = ((msw & 0x7ff00000) >> 20) - 1023;

  if (exponent_less_1023 < 20)
  {
    if (exponent_less_1023 < 0)
    {
      msw &= 0x80000000;
      if (exponent_less_1023 == -1)
        /* Result is +1.0 or -1.0. */
        msw |= (1023 << 20);
      lsw = 0;
    }
    else
    {
      ULONG exponent_mask = 0x000fffff >> exponent_less_1023;
      if ((msw & exponent_mask) == 0 && lsw == 0)
        /* x in an integral value. */
        return x;

      msw += 0x00080000 >> exponent_less_1023;
      msw &= ~exponent_mask;
      lsw = 0;
    }
  }
  else if (exponent_less_1023 > 51)
  {
    if (exponent_less_1023 == 1024)
      /* x is NaN or infinite. */
      return x + x;
    else
      return x;
  }
  else
  {
    ULONG exponent_mask = 0xffffffff >> (exponent_less_1023 - 20);
    ULONG tmp;

    if ((lsw & exponent_mask) == 0)
      /* x is an integral value. */
      return x;

    tmp = lsw + (1 << (51 - exponent_less_1023));
    if (tmp < lsw)
      msw += 1;
    lsw = tmp;

    lsw &= ~exponent_mask;
  }
  INSERT_WORDS(x, msw, lsw);

  return x;
}
