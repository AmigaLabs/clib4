/*
 * $Id: string_strsep.c,v 1.0 2021-01-21 11:20:32 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
strsep(char **src, const char *delim)
{
    char *result = NULL;
    char *string;

    string = (*src);
    if (string != NULL)
    {
        char *token = string;
        char c;

        while (TRUE)
        {
            c = (*string++);

            /* Reached the end of the string? */
            if (c == '\0')
            {
                /* No further string data available. */
                (*src) = NULL;

                result = token;
                break;
            }

            /* Is that character a delim? */
            if (strchr(delim, c) != NULL)
            {
                /* NUL-terminate the string, overwriting
				   the delim character */
                string[-1] = '\0';

                /* Scanning can resume with the next following
				   character. */
                (*src) = string;

                result = token;
                break;
            }
        }
    }

    return (result);
}