/*
 * $Id: stdlib_strtod.c,v 1.12 2006-09-25 14:51:15 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */


double
strtod(const char *str, char **ptr) {
    const char *stop = str;
    double sum = 0.0;
    double result;
    int is_negative;
    char c;

    ENTER();

    SHOWSTRING(str);
    SHOWPOINTER(ptr);

    assert(str != NULL);

    if (str == NULL) {
        SHOWMSG("invalid str parameter");

        __set_errno(EFAULT);

        result = HUGE_VAL;
        goto out;
    }

    /* Skip all leading blanks. */
    while ((c = (*str)) != '\0') {
        if (NOT isspace(c))
            break;

        str++;
    }

    /* The first character may be a sign. */
    if ((*str) == '-') {
        /* It's a negative number. */
        is_negative = 1;

        str++;
    } else {
        /* It's not going to be negative. */
        is_negative = 0;

        /* But there may be a sign we will choose to
           ignore. */
        if ((*str) == '+')
            str++;
    }

    /* We begin by checking for the "inf" and "nan" strings. */
    if (strcasecmp(str, "inf") == SAME || strcasecmp(str, "infinity") == SAME) {
        SHOWMSG("infinity");

        str += strlen(str);

        sum = __stdlib_inf();

        stop = str;
    } else if (strncasecmp(str, "nan", 3) == SAME && (str[3] == '(' || str[3] == '\0')) {
        SHOWMSG("not a number");

        str += 3;

        /* Does NaN data follow the header? If so, skip it. */
        if ((*str) == '(') {
            while ((*str) != '\0' && (*str) != ')')
                str++;

            if ((*str) == ')')
                str++;
        }

        sum = __stdlib_nan(NULL);

        stop = str;
    } else {
        size_t num_digits_converted = 0;
        int decimal_point_matches;
        double new_sum;
        int error = 0;
        int radix;

        /* Check for the hex prefix. */
        if (strncasecmp(str, "0x", 2) == SAME) {
            str += 2;

            radix = 16;
        } else {
            radix = 10;
        }

        /* Convert all the digits preceding the decimal point. */
        while ((c = (*str)) != '\0') {
            if ('0' <= c && c <= '9')
                c = c - '0';
            else if ('a' <= c && c <= 'f')
                c = c - 'a' + 10;
            else if ('A' <= c && c <= 'F')
                c = c - 'A' + 10;
            else
                c = radix;

            if (c >= radix)
                break;

            str++;

            if (error == 0) {
                new_sum = (radix * sum) + c;
                if (new_sum < sum) /* overflow? */
                    error = ERANGE;
                else
                    sum = new_sum;
            }

            num_digits_converted++;
        }

        /* Did we find the decimal point? We accept both the
           locale configured decimal point and the plain old
           dot. */
        decimal_point_matches = 0;

        __locale_lock();

        if (__CLIB2->__locale_table[LC_NUMERIC] != NULL) {
            const char *point;

            point = (const char *) __CLIB2->__locale_table[LC_NUMERIC]->loc_DecimalPoint;
            if ((*point) == (*str)) {
                decimal_point_matches = 1;

                /* Skip the decimal point. */
                str++;
            }
        }

        __locale_unlock();

        if (NOT decimal_point_matches) {
            if ((*str) == '.') {
                decimal_point_matches = 1;

                /* Skip the decimal point. */
                str++;
            }
        }

        if (decimal_point_matches) {
            double divisor = 1.0 / radix;

            /* Process all digits following the decimal point. */
            while ((c = (*str)) != '\0') {
                if ('0' <= c && c <= '9')
                    c = c - '0';
                else if ('a' <= c && c <= 'f')
                    c = c - 'a' + 10;
                else if ('A' <= c && c <= 'F')
                    c = c - 'A' + 10;
                else
                    c = radix;

                if (c >= radix)
                    break;

                str++;

                if (error == 0 && divisor != 0.0) {
                    new_sum = sum + c * divisor;
                    if (new_sum < sum) /* overflow? */
                        error = ERANGE;
                    else
                        sum = new_sum;

                    divisor = divisor / radix;
                }

                num_digits_converted++;
            }
        }

        /* If there is a scale indicator attached, process it. */
        if ((radix == 10 && ((*str) == 'e' || (*str) == 'E')) ||
            (radix == 16 && ((*str) == 'p' || (*str) == 'P'))) {
            int exponent_is_negative;
            int new_exponent;
            int exponent = 0;
            int exponent_radix;

            /* If we are processing a hexadecimal encoded
               floating point number, switch to a binary
               exponent. */
            if (radix == 16)
                exponent_radix = 2;
            else
                exponent_radix = 10;

            /* Skip the indicator. */
            str++;

            /* Take care of the exponent's sign. */
            if ((*str) == '-') {
                exponent_is_negative = 1;
                str++;
            } else {
                exponent_is_negative = 0;

                if ((*str) == '+')
                    str++;
            }

            /* Again, process all digits to follow. */
            while ((c = (*str)) != '\0') {
                if ('0' <= c && c <= '9')
                    c -= '0';
                else
                    c = exponent_radix;

                if (c >= exponent_radix)
                    break;

                str++;

                if (error == 0) {
                    new_exponent = (exponent_radix * exponent) + c;
                    if (new_exponent < exponent) /* overflow? */
                        error = ERANGE;
                    else
                        exponent = new_exponent;
                }
            }

            /* If the exponent is valid, scale the number accordingly. */
            if (exponent != 0) {
                if (exponent_is_negative) {
                    double divisor;

                    /* A negative exponent means division. */
                    divisor = __stdlib_pow((double) radix, (double) exponent);
                    if (divisor != 0.0) {
                        new_sum = sum / divisor;
                        if (new_sum == 0.0 && sum != 0.0)
                            error = ERANGE;
                        else
                            sum = new_sum;
                    } else {
                        error = ERANGE;
                    }
                } else {
                    /* A positive exponent means multiplication. */
                    new_sum = sum * __stdlib_pow((double) radix, (double) exponent);
                    if (new_sum < sum)
                        error = ERANGE;
                    else
                        sum = new_sum;
                }
            }
        }

        if (num_digits_converted == 0) {
            sum = 0;
        } else {
            if (error != 0) {
                __set_errno(error);

                sum = HUGE_VAL;
            }

            stop = str;
        }
    }

    if (is_negative)
        sum = (-sum);

    result = sum;

out:

    /* If desired, remember where we stopped reading the
       number from the buffer. */
    if (ptr != NULL)
        (*ptr) = (char *) stop;

    RETURN(result);
    return (result);
}
