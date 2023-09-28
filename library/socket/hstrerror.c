/*
 * $Id: socket_hstrerror.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

const char *
hstrerror(int error_number) {
    const char *result;

    if (error_number < 1 || error_number > 4) {
        static char error_buffer[80];
        char number[30];
        char *s = number;
        int is_negative = 0;
        unsigned int n;
        int i, len, c;

        /* We convert the error number into in an unsigned
           integer, so that numbers such as 0x80000000
           can come out of the conversion. */
        if (error_number < 0) {
            is_negative = 1;

            n = (-error_number);
        } else {
            n = error_number;
        }

        /* Convert the error number into a string of digits. */
        len = 0;

        do {
            (*s++) = '0' + (n % 10);
            n /= 10;
            len++;
        } while (n > 0 && len < (int) sizeof(number) - 1);

        /* Add the sign, if necessary. */
        if (is_negative && len < (int) sizeof(number) - 1) {
            (*s++) = '-';
            len++;
        }

        (*s) = '\0';

        /* Reverse the string in place. */
        for (i = 0; i < len / 2; i++) {
            c = number[len - 1 - i];
            number[len - 1 - i] = number[i];
            number[i] = c;
        }

        strcpy(error_buffer, "Unknown resolver error ");
        strcat(error_buffer, number);

        result = error_buffer;
    } else {
        switch (error_number) {
            case 1: /* HOST_NOT_FOUND */
                result = "Unknown host";
                break;
            case 2: /* TRY_AGAIN */
                result = "Host name lookup failure; try again";
                break;
            case 3: /* NO_RECOVERY */
                result = "Unknown server error";
                break;
            case 4: /* NO_ADDRESS */
                result = "No address associated with name";
                break;
            default:
                result = "Unknown error";
                break;
        }
    }

    return (result);
}
