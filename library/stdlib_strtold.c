/*
 * $Id: stdlib_strtold.c,v 1.0 2021-01-18 12:04:26 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _STDLIB_NULL_POINTER_CHECK_H
#include "stdlib_null_pointer_check.h"
#endif /* _STDLIB_NULL_POINTER_CHECK_H */

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/****************************************************************************/

#if defined(USE_64_BIT_INTS)

STATIC const double powerof10[] = {1.e1, 1.e2, 1.e4, 1.e8, 1.e16, 1.e32};
STATIC const double digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

long double
strtold(const char *str, char **ptr)
{
    long double result = 0;
    char cp;
    const char *fst = str;
    int exp = 0;   /* EXPONENT              */
    int count = 0; /* EXPONENT CALCULATION  */
    int value = 0; /* SUCCESSFUL PARSE      */
    int sign;
    int plus_or_minus = 0; /* READ IN EXPONENT SIGN (+/-) */

    ENTER();

    SHOWSTRING(str);
    SHOWVALUE(base);

    assert(str != NULL);

#if defined(CHECK_FOR_NULL_POINTERS)
    {
        if (str == NULL)
        {
            SHOWMSG("invalid str parameter");

            __set_errno(EFAULT);
            goto out;
        }
    }
#endif /* CHECK_FOR_NULL_POINTERS */

    while (isspace(*fst))
        ++fst; /* SKIP WHITE SPACE */
    if ((sign = ((cp = *fst) == '-')) || (cp == '+'))
    {
        ++fst;
        value = 1;
    }

    /*----------------------------------------------------------------------*/
    /* READ IN FRACTIONAL PART OF NUMBER, UNTIL AN 'E' IS REACHED.          */
    /* COUNT DIGITS AFTER DECIMAL POINT.                                    */
    /*----------------------------------------------------------------------*/
    for (; isdigit(cp = *fst); ++fst)
    {
        result = result * 10 + digits[cp - '0'];
        value = 1;
    }

    if (cp == '.')
    {
        while (isdigit(cp = *++fst))
        {
            result = result * 10 + digits[cp - '0'];
            value = 1;
            --exp;
        }
    }

    if (sign)
        result = -result; /* IF NEGATIVE NUMBER, REVERSE SIGN */

    /*----------------------------------------------------------------------*/
    /* READ IN EXPLICIT EXPONENT AND CALCULATE REAL EXPONENT.               */
    /* IF EXPONENT IS BOGUS (i.e. "1.234empty" or "1.234e+mpty") RESTORE    */
    /* BOGUS EXPONENT BACK ONTO RETURNED STRING (ptr).                   */
    /*----------------------------------------------------------------------*/
    if (value && toupper(*fst) == 'E')
    {
        if ((sign = ((cp = *++fst) == '-')) || (cp == '+'))
        {
            cp = *++fst;
            plus_or_minus = 1;
        }

        if (!isdigit(cp))
        {
            if (plus_or_minus)
                *--fst;
            *--fst;
            goto skip_loop;
        }

        for (count = 0; isdigit(cp); cp = *++fst)
        {
            if ((INT_MAX - abs(exp) - (cp - '0')) / 10 > count)
            {
                count *= 10;
                count += cp - '0';
            }
            else
            {
                count = INT_MAX - exp;
                break;
            }
        }

    skip_loop:

        if (sign)
            exp -= count;
        else
            exp += count;
    }

    /*----------------------------------------------------------------------*/
    /* ADJUST NUMBER BY POWERS OF TEN SPECIFIED BY FORMAT AND EXPONENT.     */
    /*----------------------------------------------------------------------*/
    if (result != 0.0)
    {
        if (exp > LDBL_MAX_10_EXP)
        {
            __set_errno(ERANGE);
            result = (result < 0) ? -HUGE_VALL : HUGE_VALL;
        }
        else if (exp < LDBL_MIN_10_EXP)
        {
            __set_errno(ERANGE);
            result = 0.0;
        }
        else if (exp < 0)
        {
            for (count = 0, exp = -exp; exp; count++, exp >>= 1)
            {
                if (exp & 1)
                    result /= powerof10[count];
            }
        }
        else
        {
            for (count = 0; exp; count++, exp >>= 1)
            {
                if (exp & 1)
                    result *= powerof10[count];
            }
        }
    }

    if (ptr)
        *ptr = (char *)(value ? fst : str);
    
    RETURN(result);
    return result;
}

#endif
