/*
 * $Id: string_strerror.c,v 1.4 2006-09-17 17:36:42 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

static void
number_to_string(int number, char *string, size_t string_size) {
    int is_negative = 0;
    char *s = string;
    unsigned int n;
    size_t i, len;
    char c;

    /* We convert the error number into in an unsigned
       integer, so that numbers such as 0x80000000
       can come out of the conversion. */
    if (number < 0) {
        is_negative = 1;

        n = (-number);
    } else {
        n = number;
    }

    /* Convert the error number into a string of digits. */
    len = 0;

    do {
        (*s++) = '0' + (n % 10);
        n /= 10;
        len++;
    } while (n > 0 && len < string_size - 1);

    /* Add the sign, if necessary. */
    if (is_negative && len < string_size - 1) {
        (*s++) = '-';
        len++;
    }

    (*s) = '\0';

    /* Reverse the string in place. */
    for (i = 0; i < len / 2; i++) {
        c = string[len - 1 - i];
        string[len - 1 - i] = string[i];
        string[i] = c;
    }
}

char *
strerror(int error_number) {
    static char error_message[80];

    /* NOTE: We are making the assumption that if strerror_r() fails, it
             will fail only because the error code is unknown, and not
             because the size of the supplied buffer is too small. */

    if (strerror_r(error_number, error_message, sizeof(error_message)) != 0) {
        char number[20];

        number_to_string(error_number, number, sizeof(number));

        strcpy(error_message, "Unknown error ");
        strcat(error_message, number);
    }

    return (error_message);
}
