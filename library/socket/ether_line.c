/*
 * $Id: socket_ether_line.c,v 1.0 2023-06-10 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
ether_line(const char *line, struct ether_addr *addr, char *hostname) {
    for (size_t cnt = 0; cnt < 6; ++cnt) {
        unsigned int number;
        char ch;

        ch = tolower(*line++);
        if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
            return -1;
        number = isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

        ch = tolower(*line);
        if ((cnt < 5 && ch != ':') || (cnt == 5 && ch != '\0' && !isspace(ch))) {
            ++line;
            if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
                return -1;
            number <<= 4;
            number += isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

            ch = *line;
            if (cnt < 5 && ch != ':')
                return -1;
        }

        /* Store result.  */
        addr->ether_addr_octet[cnt] = (unsigned char) number;

        /* Skip ':'.  */
        if (ch != '\0')
            ++line;
    }

    /* Skip initial whitespace.  */
    while (isspace(*line))
        ++line;

    if (*line == '#' || *line == '\0')
        /* No hostname.  */
        return -1;

    /* The hostname is up to the next non-space character.  */
    /* XXX This can cause trouble because the hostname might be too long
       but we have no possibility to check it here.  */
    while (*line != '\0' && *line != '#' && !isspace(*line))
        *hostname++ = *line++;
    *hostname = '\0';

    return 0;
}