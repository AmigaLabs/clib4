#ifndef _CRYPT_PRIVATE_H
#define _CRYPT_PRIVATE_H

#include <crypt.h>

char *__crypt_r(const char *, const char *, struct crypt_data *);

char *__crypt_des(const char *, const char *, char *);
char *__crypt_md5(const char *, const char *, char *);
char *__crypt_blowfish(const char *, const char *, char *);
char *__crypt_sha256(const char *, const char *, char *);
char *__crypt_sha512(const char *, const char *, char *);

#endif