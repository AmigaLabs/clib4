/*
 * $Id: crypt_crypt.c,v 1.00 2022-07-30 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <crypt.h>

char *
crypt(const char *key, const char *salt) {
    /* This buffer is sufficiently large for all
     * currently-supported hash types. It needs to be updated if
     * longer hashes are added. The cast to struct crypt_data * is
     * purely to meet the public API requirements of the crypt_r
     * function; the implementation of crypt_r uses the object
     * purely as a char buffer. */
    static char buf[128] = {0};
    return crypt_r(key, salt, (struct crypt_data *) buf);
}
