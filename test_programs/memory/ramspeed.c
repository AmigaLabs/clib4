#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned int unalign;

struct test_fct {
    const char *name;

    unsigned int (*f)(unsigned int, unsigned int);
};

#ifndef CLIB2
static inline unsigned long long rdtsc() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
#endif

unsigned int bench_memcpy(unsigned int loop, unsigned int size) {
    unsigned long long before, after;
    unsigned int i;
    void *src, *dst;

    posix_memalign(&src, 64, size);
    posix_memalign(&dst, 64, size + unalign);

    /* ensure the pages are allocated */
    memset(src, 0, size);
    memset(dst, 0, size);

    before = rdtsc();
    for (i = 0; i < loop; i++) {
        memcpy(dst + unalign, src, size);
        asm("":: : "memory");
    }
    after = rdtsc();

    free(src);
    free(dst);
    return after - before;
}

unsigned int bench_memchr(unsigned int loop, unsigned int size) {
    unsigned long long before, after;
    unsigned int i;
    char *dst;

    dst = malloc(size);

    /* ensure the pages are allocated */
    memset(dst, 0, size);

    before = rdtsc();
    for (i = 0; i < loop; i++) {
        if (memchr(dst, i | 1, size))
            return 0;
        asm("":: : "memory");
    }

    after = rdtsc();

    free(dst);
    return after - before;
}

unsigned int bench_memset(unsigned int loop, unsigned int size) {
    unsigned long long before, after;
    unsigned int i;
    char *dst;

    dst = malloc(size);

    /* ensure the pages are allocated */
    memset(dst, 0, size);

    before = rdtsc();
    for (i = 0; i < loop; i++) {
        memset(dst, i, size);
        asm("":: : "memory");
    }
    after = rdtsc();

    free(dst);
    return after - before;
}

static struct test_fct test_fcts[] = {
        {"bench_memchr", bench_memchr},
        {"bench_memset", bench_memset},
        {"bench_memcpy", bench_memcpy},
        {NULL, NULL}
};

void run_bench(unsigned int loop, unsigned int size) {
    unsigned int usecs;
    struct test_fct *t = test_fcts;

    printf("Running tests with %d loops of %d bytes\n", loop, size);

    while (t->name && t->f) {
        usecs = t->f(loop, size);
        printf("%s: %ld us for %d loops of %d bytes = %lld kB/s\n",
               t->name,
               usecs, loop, size, (unsigned long long) 1024ULL * loop * size / usecs);
        t++;
    }
}

int main(int argc, char **argv) {
    unsigned int size;
    unsigned int loop;

    loop = 10;
    size = 16777216;

    if (argc > 1)
        loop = atoi(argv[1]);

    if (argc > 2)
        size = atoi(argv[2]);

    if (argc > 3)
        unalign = atoi(argv[3]);

    run_bench(loop, size);
    exit(0);
}