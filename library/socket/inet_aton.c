/*
 * $Id: socket_inet_aton.c,v 1.4 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <ctype.h>

/* 
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */
int
inet_aton(const char *cp, struct in_addr *addr) {
    ULONG val = 0;
    int base, n;
    char c;
    LONG parts[4];
    LONG *pp = parts;
    int result = 1;

    ENTER();

    assert(cp != NULL && addr != NULL);

    if (cp == NULL || addr == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        result = 0;
        goto out;
    }

    for (;;) {
        /*
         * Collect number up to ``.''.
         * Values are specified as for C:
         * 0x=hex, 0=octal, other=decimal.
         */
        val = 0;
        base = 10;
        if (*cp == '0') {
            if (*++cp == 'x' || *cp == 'X')
                base = 16, cp++;
            else
                base = 8;
        }
        while ((c = *cp) != '\0') {
            if (isascii(c) && isdigit(c)) {
                val = (val * base) + (c - '0');
                cp++;
                continue;
            }
            if (base == 16 && isascii(c) && isxdigit(c)) {
                val = (val << 4) +
                      (c + 10 - (islower(c) ? 'a' : 'A'));
                cp++;
                continue;
            }
            break;
        }
        if (*cp == '.') {
            /*
             * Internet format:
             *	a.b.c.d
             *	a.b.c	(with c treated as 16-bits)
             *	a.b	(with b treated as 24 bits)
             */
            if (pp >= parts + 3 || val > 0xff) {
                result = 0;
                goto out;
            }
            *pp++ = val, cp++;
        } else
            break;
    }
    /*
     * Check for trailing characters.
     */
    if (*cp && (!isascii(*cp) || !isspace(*cp))) {
        result = 0;
        goto out;
    }
    /*
     * Concoct the address according to
     * the number of parts specified.
     */
    n = pp - parts + 1;
    switch (n) {
        case 1: /* a -- 32 bits */
            break;
        case 2: /* a.b -- 8.24 bits */
            if (val > 0xffffff) {
                result = 0;
                goto out;
            }
            val |= parts[0] << 24;
            break;
        case 3: /* a.b.c -- 8.8.16 bits */
            if (val > 0xffff) {
                result = 0;
                goto out;
            }
            val |= (parts[0] << 24) | (parts[1] << 16);
            break;
        case 4: /* a.b.c.d -- 8.8.8.8 bits */
            if (val > 0xff) {
                result = 0;
                goto out;
            }
            val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
            break;
        default:
            result = 0;
            goto out;
    }
    if (addr)
        addr->s_addr = val;

out:

    RETURN(result);
    return result;
}
