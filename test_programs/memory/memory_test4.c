#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#if !defined(MEMCPY) && !defined(MEMSET) && !defined(MEMMOVE)
#error please define at least one from MEMCPY, MEMSET or MEMMOVE
#endif

#define ARRAY_SIZE(x) (sizeof (x) / sizeof (x[0]))

int
main(int argc, char *argv[]) {
    static char msrc[8192] __attribute__ ((aligned (32)));
    static char mdst[8192] __attribute__ ((aligned (32)));
    const int offs[] = {0, 8, 9};
    unsigned long size, loop;
    int i, j;

    if (argc < 3) {
        fprintf(stderr, "%s SIZE LOOP\n", argv[0]);
        return -1;
    }

    size = atoi(argv[1]);
    loop = atoi(argv[2]);

    for (i = 0; i < ARRAY_SIZE (offs); i++) {
        struct timeval start, stop;
        unsigned long elapsed;
        double speed;

        gettimeofday(&start, NULL);
        for (j = 0; j < loop; j++) {
#if defined(MEMSET)
            memset (&mdst[offs[i]], 0, size);
#elif defined(MEMCPY)
            memcpy (&mdst[offs[i]], msrc, size);
#elif defined(MEMMOVE)
            memmove (&mdst[128 + offs[i]], mdst, size);
#endif
        }
        gettimeofday(&stop, NULL);
        elapsed = (stop.tv_sec - start.tv_sec) * 1000000UL +
                  stop.tv_usec - start.tv_usec;
        speed = (double) (((size * loop) / (1024 * 1024UL))) /
                elapsed * 1000000UL;
        printf("%s size %lu src_off 0 dst_off %d speed %5.3f MB/s\n",
               argv[0], size, offs[i], speed);
    }

    return 0;
}