/*
 * $Id: time_numbertostring.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

char *
__number_to_string(
        unsigned int number,
        char *string,
        size_t max_len,
        size_t min_len) {
    char *s = string;
    size_t len, i;
    char c;

    assert(string != NULL && max_len >= min_len);

    /* One off for the terminating NUL. */
    if (max_len > 0)
        max_len--;

    len = 0;

    /* Convert the number digit by digit, building the
     * string in reverse order.
     */
    do {
        if (max_len == 0)
            break;

        max_len--;

        (*s++) = '0' + (number % 10);
        number /= 10;

        len++;
    } while (number > 0);

    /* Pad the string to the requested length, if necessary. */
    while (max_len > 0 && len < min_len) {
        max_len--;

        (*s++) = '0';
        len++;
    }

    (*s) = '\0';

    /* Reverse the string in place. */
    for (i = 0; i < len / 2; i++) {
        c = string[len - 1 - i];
        string[len - 1 - i] = string[i];
        string[i] = c;
    }

    return (string);
}
