#include "memcmp.h"

#include <string.h> // memcmp

bool memcmp_stdlib(const void *const s1, const void *const s2, const size_t n)
{
    return memcmp(s1, s2, n) == 0;
}