#include "memcmp.h"

bool memcmp_naive(const void *const s1, const void *const s2, const size_t n)
{
    const unsigned char *const c1 = s1;
    const unsigned char *const c2 = s2;
    for (size_t i = 0; i < n; ++i)
    {
        if (c1[i] != c2[i])
            return false;
    }
    return true;
}