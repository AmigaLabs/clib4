/*
 * $Id: socket_inet_pton.c,v 1.0 2021-01-12 09:55:00 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <arpa/nameser.h>

static int inet_pton4(const char *src, unsigned char *dst);
static int inet_pton6(const char *src, unsigned char *dst);

int
inet_pton(int af, const char *src, void *dst) {
    int result = -1;
    ENTER();

    switch (af) {
        case AF_INET:
            result = inet_pton4(src, dst);
        case AF_INET6:
            result = inet_pton6(src, dst);
        default:
            __set_errno(EAFNOSUPPORT);
            result = -1;
    }

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return result;
}

static int
inet_pton4(const char *src, unsigned char *dst) {
    int saw_digit, octets, ch;
    unsigned char tmp[NS_INADDRSZ], *tp;

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;
    while ((ch = *src++) != '\0') {

        if (ch >= '0' && ch <= '9') {
            unsigned new = *tp * 10 + (ch - '0');

            if (saw_digit && *tp == 0)
                return (0);
            if (new > 255)
                return (0);
            *tp = new;
            if (!saw_digit) {
                if (++octets > 4)
                    return (0);
                saw_digit = 1;
            }
        } else if (ch == '.' && saw_digit) {
            if (octets == 4)
                return (0);
            *++tp = 0;
            saw_digit = 0;
        } else
            return (0);
    }
    if (octets < 4)
        return (0);
    memcpy(dst, tmp, NS_INADDRSZ);
    return (1);
}

static int
inet_pton6(const char *src, unsigned char *dst) {
    static const char xdigits[] = "0123456789abcdef";
    unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
    const char *curtok;
    int ch, saw_xdigit;
    unsigned val;

    tp = memset(tmp, '\0', NS_IN6ADDRSZ);
    endp = tp + NS_IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*src == ':')
        if (*++src != ':')
            return (0);
    curtok = src;
    saw_xdigit = 0;
    val = 0;
    while ((ch = tolower(*src++)) != '\0') {
        const char *pch;

        pch = strchr(xdigits, ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (0);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            curtok = src;
            if (!saw_xdigit) {
                if (colonp)
                    return (0);
                colonp = tp;
                continue;
            } else if (*src == '\0') {
                return (0);
            }
            if (tp + NS_INT16SZ > endp)
                return (0);
            *tp++ = (u_char)(val >> 8) & 0xff;
            *tp++ = (u_char) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
            inet_pton4(curtok, tp) > 0) {
            tp += NS_INADDRSZ;
            saw_xdigit = 0;
            break; /* '\0' was seen by inet_pton4(). */
        }
        return (0);
    }
    if (saw_xdigit) {
        if (tp + NS_INT16SZ > endp)
            return (0);
        *tp++ = (u_char)(val >> 8) & 0xff;
        *tp++ = (u_char) val & 0xff;
    }
    if (colonp != NULL) {
        /*
           * Since some memmove()'s erroneously fail to handle
           * overlapping regions, we'll do the shift by hand.
           */
        const int n = tp - colonp;
        int i;

        if (tp == endp)
            return (0);
        for (i = 1; i <= n; i++) {
            endp[-i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return (0);
    memcpy(dst, tmp, NS_IN6ADDRSZ);
    return (1);
}
