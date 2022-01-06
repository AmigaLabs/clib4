/*
 * (c) 2008, Vicent Gable
 * Reference implementation of memcpy, memmove, and a naive benchmark.
 * See http://vgable.com/blog/2008/05/24/memcopy-memmove-and-speed-over-safety/
 *
 * License: For educational purposes only.
 * Don't use this code for anything.
 * Use a more credible memmove/memcpy implementation instead.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static inline void *
memcpy_forward(void *dst, void *src, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        ((char *)dst)[i] = ((char *)src)[i];
    return dst;
}

static inline void *
memcpy_backwards(char *dst, char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
        dst[n - i - 1] = src[n - i - 1];
    return dst;
}

/*
 * To show that copying backwards is no slower then copying forwards.
 */
static inline void *
memcpy_backwards_optimized(char *dst, char *src, size_t n)
{
    if (n != 0)
    {
        do
        {
            n--;
            dst[n] = src[n];
        } while (n != 0);
    }
    return dst;
}

void *
my_memcpy(void *dst, void *src, size_t n)
{
    return memcpy_forward(dst, src, n);
}

void *
my_memmove(void *dst, void *src, size_t n)
{
    if (dst > src)
        return memcpy_backwards(dst, src, n);
    else
        return memcpy_forward(dst, src, n);
}

int main(int argc, char **argv)
{
    const int N = 1000000;
    const int SIZE = 4;
    void *src = malloc(SIZE);
    void *dst = malloc(SIZE);
    int i;

    long double memcpy_total = 0;
    for (i = 0; i < N; ++i)
    {
        clock_t start = clock();
        memcpy(dst, src, SIZE);
        memcpy_total += (clock() - start);
    }

    long double memmove_total = 0;
    for (i = 0; i < N; ++i)
    {
        clock_t start = clock();
        memmove(dst, src, SIZE);
        memmove_total += (clock() - start);
    }

    if (memmove_total > memcpy_total)
        printf("memcpy is %Lf times faster\n", memmove_total / memcpy_total);
    else
        printf("Surprise!\nmemmove is %Lf times faster then memcpy\n", memcpy_total / memmove_total);

    return 0;
}