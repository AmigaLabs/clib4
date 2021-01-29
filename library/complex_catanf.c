/*
 * Experimental: Not a part of clib2 from Olaf Barthel
 */

/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Imported and modified 2021/01/25
 * Ola Soder <rolfkopman@gmail.com>
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

/****************************************************************************/

#if defined(COMPLEX_SUPPORT)

/****************************************************************************/

static float
_redupif(float x)
{
    float t = x / (float) M_PI, dp1 = 3.14159265160560607910E0,
          dp2 = 1.98418714791870343106E-9, dp3 = 1.14423774522196636802E-17;
    long int i = t >= 0.0f ? t + 0.5f : t - 0.5f;

    t = i;
    return ((x - t * dp1) - t * dp2) - t * dp3;
}

float complex
catanf(float complex z)
{
    float x = crealf(z), y = cimagf(z), x2 = x * x, a = 1.0f - x2 - (y * y),
          w = _redupif(0.5f * atan2f(2.0f * x, a)), t = y - 1.0f;

    a = x2 + t * t;
    t = y + 1.0f;
    a = (x2 + t * t) / a;
    return CMPLXF(w, 0.25f * logf(a));
}


/****************************************************************************/

#endif /* COMPLEX_SUPPORT */
