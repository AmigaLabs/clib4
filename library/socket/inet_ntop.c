/* $Id: socket_inet_ntop.c,v 1.0 2021-11-16 10:41:15 clib4devs Exp $
 */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

typedef int verify_int_size[4 <= sizeof(int) ? 1 : -1];
static const char *inet_ntop4(const unsigned char *src, char *dst, socklen_t size);
static const char *inet_ntop6(const unsigned char *src, char *dst, socklen_t size);

const char *
inet_ntop(int af, const void *src, char *dst, socklen_t cnt) {
    const char* result = NULL;

    ENTER();
    switch (af) {
        case AF_INET:
            result = inet_ntop4(src, dst, cnt);
            break;
        case AF_INET6:
            result = inet_ntop6(src, dst, cnt);
            break;
        default:
            __set_errno(EAFNOSUPPORT);
            result = NULL;
    }

    __check_abort();

    RETURN(result);
    return result;
}


static const char *
inet_ntop4(const unsigned char *src, char *dst, socklen_t size) {
    char tmp[sizeof "255.255.255.255"];
    int len;

    len = sprintf(tmp, "%u.%u.%u.%u", src[0], src[1], src[2], src[3]);
    if (len < 0)
        return NULL;

    if ((socklen_t) len > size) {
        __set_errno(ENOSPC);
        return NULL;
    }

    return strcpy(dst, tmp);
}

static const char *
inet_ntop6(const unsigned char *src, char *dst, socklen_t size) {
    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
    struct {
        int base, len;
    } best, cur;
    unsigned int words[NS_IN6ADDRSZ / NS_INT16SZ];
    int i;

    /*
     * Preprocess:
     *      Copy the input (bytewise) array into a wordwise array.
     *      Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < NS_IN6ADDRSZ; i += 2)
        words[i / 2] = (src[i] << 8) | src[i + 1];
    best.base = -1;
    cur.base = -1;
    best.len = 0;
    cur.len = 0;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;

    /*
     * Format the result.
     */
    tp = tmp;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        /* Are we inside the best run of 0x00's? */
        if (best.base != -1 && i >= best.base && i < (best.base + best.len)) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0)
            *tp++ = ':';
        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 &&
            (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            if (!inet_ntop4(src + 12, tp, sizeof tmp - (tp - tmp)))
                return (NULL);
            tp += strlen(tp);
            break;
        }
        {
            int len = sprintf(tp, "%x", words[i]);
            if (len < 0)
                return NULL;
            tp += len;
        }
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) == (NS_IN6ADDRSZ / NS_INT16SZ))
        *tp++ = ':';
    *tp++ = '\0';

    /*
     * Check for overflow, copy, and we're done.
     */
    if ((socklen_t)(tp - tmp) > size) {
        __set_errno(ENOSPC);
        return NULL;
    }

    return strcpy(dst, tmp);
}
