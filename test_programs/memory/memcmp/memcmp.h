#ifndef MEMCMP_H
#define MEMCMP_H

#include <stdbool.h> // bool
#include <stddef.h>  // size_t
#include <features.h>

__BEGIN_DECLS

bool memcmp_stdlib(const void *s1, const void *s2, size_t n)__attribute__((hot));

bool memcmp_naive(const void *s1, const void *s2, size_t n)__attribute__((hot));

bool memcmp_yamiez(const void *s1, const void *s2, size_t n)__attribute__((hot));

__END_DECLS

#endif // #ifndef MEMCMP_H