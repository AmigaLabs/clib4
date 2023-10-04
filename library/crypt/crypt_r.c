/*
 * $Id: crypt_crypt_r.c,v 1.00 2022-07-30 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include "crypt_private.h"

#include <crypt.h>

char *
crypt_r(const char *key, const char *salt, struct crypt_data *data) {
    /* Per the crypt_r API, the caller has provided a pointer to
     * struct crypt_data; however, this implementation does not
     * use the structure to store any internal state, and treats
     * it purely as a char buffer for storing the result. */
    char *output = (char *) data;
    if (salt[0] == '$' && salt[1] && salt[2]) {
        if (salt[1] == '1' && salt[2] == '$')
            return __crypt_md5(key, salt, output);
        if (salt[1] == '2' && salt[3] == '$')
            return __crypt_blowfish(key, salt, output);
        if (salt[1] == '5' && salt[2] == '$')
            return __crypt_sha256(key, salt, output);
        if (salt[1] == '6' && salt[2] == '$')
            return __crypt_sha512(key, salt, output);
    }
    return __crypt_des(key, salt, output);
}
