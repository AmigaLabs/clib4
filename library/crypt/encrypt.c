/*
 * $Id: stdlib_encrypt.c,v 1.0 2022-08-06 10:36:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "crypt_des.h"

static struct expanded_key __encrypt_key;

void setkey(const char *key) {
    unsigned char bkey[8];
    int i, j;

    for (i = 0; i < 8; i++) {
        bkey[i] = 0;
        for (j = 7; j >= 0; j--, key++)
            bkey[i] |= (uint32_t)(*key & 1) << j;
    }

    __des_setkey(bkey, &__encrypt_key);
}

void encrypt(char *block, int edflag) {
    struct expanded_key decrypt_key, *key;
    uint32_t b[2];
    int i, j;
    char *p;

    p = block;
    for (i = 0; i < 2; i++) {
        b[i] = 0;
        for (j = 31; j >= 0; j--, p++)
            b[i] |= (uint32_t)(*p & 1) << j;
    }

    key = &__encrypt_key;
    if (edflag) {
        key = &decrypt_key;
        for (i = 0; i < 16; i++) {
            decrypt_key.l[i] = __encrypt_key.l[15 - i];
            decrypt_key.r[i] = __encrypt_key.r[15 - i];
        }
    }

    __do_des(b[0], b[1], b, b + 1, 1, 0, key);

    p = block;
    for (i = 0; i < 2; i++)
        for (j = 31; j >= 0; j--)
            *p++ = b[i] >> j & 1;
}
