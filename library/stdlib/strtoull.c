/*
 * $Id: stdlib_strtoull.c,v 1.6 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

unsigned long long
strtoull(const char *str, char **ptr, int base) {
    const char *stop = str;
    size_t num_digits_converted = 0;
    BOOL is_negative;
    unsigned long long result = 0;
    unsigned long long new_sum;
    unsigned long long sum;
    char c;

    ENTER();

    SHOWSTRING(str);
    SHOWPOINTER(ptr);
    SHOWVALUE(base);

    assert(str != NULL && base >= 0);

    if (str == NULL) {
        SHOWMSG("invalid str parameter");

        __set_errno(EFAULT);
        goto out;
    }

    if (base < 0) {
        SHOWMSG("invalid base parameter");

        __set_errno(ERANGE);
        goto out;
    }

    /* Skip all leading blanks. */
    while ((c = (*str)) != '\0') {
        if (NOT isspace(c))
            break;

        str++;
    }

    /* The first character may be a sign. */
    if (c == '-') {
        /* It's a negative number. */
        is_negative = TRUE;

        str++;
    } else {
        /* It's not going to be negative. */
        is_negative = FALSE;

        /* But there may be a sign we will choose to
           ignore. */
        if (c == '+')
            str++;
    }

    c = (*str);

    /* There may be a leading '0x' to indicate that what
       follows is a hexadecimal number. */
    if (base == 0 || base == 16) {
        if ((c == '0') && (str[1] == 'x' || str[1] == 'X')) {
            base = 16;

            str += 2;

            c = (*str);
        }
    }

    /* If we still don't know what base to use and the
       next letter to follow is a zero then this is
       probably a number in octal notation. */
    if (base == 0) {
        if (c == '0')
            base = 8;
        else
            base = 10;
    }

    sum = 0;

    if (1 <= base && base <= 36) {
        while (c != '\0') {
            if ('0' <= c && c <= '9')
                c -= '0';
            else if ('a' <= c)
                c -= 'a' - 10;
            else if ('A' <= c)
                c -= 'A' - 10;
            else
                break;

            /* Ignore invalid numbers. */
            if (c >= base)
                break;

            new_sum = base * sum + c;
            if (new_sum < sum) /* overflow? */
            {
                __set_errno(ERANGE);

                result = ULONG_MAX;

                goto out;
            }

            sum = new_sum;

            str++;

            c = (*str);

            num_digits_converted++;
        }
    }

    /* Did we convert anything? */
    if (num_digits_converted == 0)
        goto out;

    if (is_negative)
        result = (-sum);
    else
        result = sum;

    stop = str;

out:

    /* If desired, remember where we stopped reading the
       number from the buffer. */
    if (ptr != NULL)
        (*ptr) = (char *) stop;

    RETURN(result);
    return (result);
}
