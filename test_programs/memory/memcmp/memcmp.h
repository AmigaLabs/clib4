#ifndef MEMCMP_H
#define MEMCMP_H

#include <stdbool.h> // bool
#include <stddef.h>  // size_t

#ifdef __cplusplus
extern "C"
{
#endif

    bool
    memcmp_stdlib(const void *s1, const void *s2, size_t n)
        __attribute__((hot));

    bool
    memcmp_naive(const void *s1, const void *s2, size_t n)
        __attribute__((hot));

    bool
    memcmp_yamiez(const void *s1, const void *s2, size_t n)
        __attribute__((hot));

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef MEMCMP_H