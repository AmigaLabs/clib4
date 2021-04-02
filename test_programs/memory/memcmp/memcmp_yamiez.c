#include "memcmp.h"

#include <stdint.h> // uintptr_t

bool memcmp_yamiez(const void *const s1, const void *const s2, size_t n)
{
    typedef unsigned char byte_type;
    typedef unsigned long word_type;
    const size_t word_size = sizeof(word_type);
    const size_t word_align = (word_size >= 8) ? word_size : 8;
    const uintptr_t align_mask = word_align - 1;
    const byte_type *buf1 = s1;
    const byte_type *buf2 = s2;
    const uintptr_t addr1 = (uintptr_t)s1;
    const uintptr_t addr2 = (uintptr_t)s2;
    if ((addr1 & align_mask) == (addr2 & align_mask))
    {
        const uintptr_t skip = word_align - (addr1 & align_mask);
        for (uintptr_t i = 0; i < skip; ++i)
        {
            if (*buf1++ != *buf2++)
                return false;
            --n;
        }
        const word_type *wbuf1 = (const word_type *)buf1;
        const word_type *wbuf2 = (const word_type *)buf2;
        while (n >= word_size)
        {
            if (*wbuf1++ != *wbuf2++)
                return false;
            n -= word_size;
        }
        buf1 = (const byte_type *)wbuf1;
        buf2 = (const byte_type *)wbuf2;
    }
    while (n--)
    {
        if (*buf1++ != *buf2++)
            return false;
    }
    return true;
}