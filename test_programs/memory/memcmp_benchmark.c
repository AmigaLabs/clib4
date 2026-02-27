#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#define ITERATIONS 50000000

int mycmp(const unsigned char *cmp1, const unsigned char *cmp2, unsigned int length);
int memcmp_naive(const void *const s1, const void *const s2, const size_t n);
int memcmp_yamiez(const void *const s1, const void *const s2, size_t n);

int main(int argc, const char *argv[]) {
    uint64_t i;
    clock_t begin;
    double time_spent;

    /* Test the native memcmp() speed by running a simple non-matching comparison a billion times, which takes a few seconds */
    begin = clock();
    for (i = 0; i < ITERATIONS; i++) {
        memcmp("111111111111", "111111111111", 12);
    }
    time_spent = (double) (clock() - begin) / CLOCKS_PER_SEC;
    printf("Native memcmp:\t\t%f\n", time_spent);

    /* Test the memcmp_naive() speed */
    begin = clock();
    for (i = 0; i < ITERATIONS; i++) {
        memcmp_naive("111111111111", "111111111111", 12);
    }
    time_spent = (double) (clock() - begin) / CLOCKS_PER_SEC;
    printf("Modified memcmp:\t%f\n", time_spent);

    /* Test the memcmp_yamiez() speed */
    begin = clock();
    for (i = 0; i < ITERATIONS; i++) {
        memcmp_yamiez("111111111111", "111111111111", 12);
    }
    time_spent = (double) (clock() - begin) / CLOCKS_PER_SEC;
    printf("Modified memcmp:\t%f\n", time_spent);

    /* Test the modified memcmp() speed */
    begin = clock();
    for (i = 0; i < ITERATIONS; i++) {
        mycmp("111111111111", "111111111111", 12);
    }
    time_spent = (double) (clock() - begin) / CLOCKS_PER_SEC;
    printf("Modified memcmp:\t%f\n", time_spent);


    return 0;
}


/*  The modified memcmp. It simply typecasts the values to int's and return the difference if the first four bytes are different and otherwise returns the native memcmp()'s return value */
int mycmp(const unsigned char *cmp1, const unsigned char *cmp2, unsigned int length) {
    if (length >= 4) {
        int difference = *(int *) cmp1 - *(int *) cmp2;
        if (difference)
            return difference;
    }

    return memcmp(cmp1, cmp2, length);
}

int memcmp_naive(const void *const s1, const void *const s2, const size_t n) {
    const unsigned char *const c1 = s1;
    const unsigned char *const c2 = s2;
    for (size_t i = 0; i < n; ++i) {
        if (c1[i] != c2[i])
            return 0;
    }
    return 1;
}

int memcmp_yamiez(const void *const s1, const void *const s2, size_t n) {
    typedef unsigned char byte_type;
    typedef unsigned long word_type;
    const size_t word_size = sizeof(word_type);
    const size_t word_align = (word_size >= 8) ? word_size : 8;
    const uintptr_t align_mask = word_align - 1;
    const byte_type *buf1 = s1;
    const byte_type *buf2 = s2;
    const uintptr_t addr1 = (uintptr_t) s1;
    const uintptr_t addr2 = (uintptr_t) s2;
    if ((addr1 & align_mask) == (addr2 & align_mask)) {
        const uintptr_t skip = word_align - (addr1 & align_mask);
        for (uintptr_t i = 0; i < skip; ++i) {
            if (*buf1++ != *buf2++)
                return 0;
            --n;
        }
        const word_type *wbuf1 = (const word_type *) buf1;
        const word_type *wbuf2 = (const word_type *) buf2;
        while (n >= word_size) {
            if (*wbuf1++ != *wbuf2++)
                return 0;
            n -= word_size;
        }
        buf1 = (const byte_type *) wbuf1;
        buf2 = (const byte_type *) wbuf2;
    }
    while (n--) {
        if (*buf1++ != *buf2++)
            return 0;
    }
    return 1;
}