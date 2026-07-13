#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#if !defined(MEMCPY) && !defined(MEMSET) && !defined(MEMMOVE)
#warning no MEMCPY, MEMSET or MEMMOVE defined. Defaulting to MEMCPY
#define MEMCPY 1
#endif

#define ARRAY_SIZE(x) (sizeof (x) / sizeof (x[0]))

int
main(int argc, char *argv[]) {
    static char msrc[8192] __attribute__ ((aligned (32)));
    static char mdst[8192] __attribute__ ((aligned (32)));
    const int offs[] = {0, 8, 9};
    const size_t msrc_size = sizeof(msrc);
    const size_t mdst_size = sizeof(mdst);
    unsigned long size, loop;
    size_t i;

    if (argc < 3) {
        fprintf(stderr, "%s SIZE LOOP\n", argv[0]);
        return -1;
    }

    {
        char *end;

        errno = 0;
        size = strtoul(argv[1], &end, 10);
        if (errno != 0 || *argv[1] == '\0' || *end != '\0' || size == 0) {
            fprintf(stderr, "invalid SIZE '%s' (must be a positive integer)\n", argv[1]);
            return -1;
        }

        errno = 0;
        loop = strtoul(argv[2], &end, 10);
        if (errno != 0 || *argv[2] == '\0' || *end != '\0' || loop == 0) {
            fprintf(stderr, "invalid LOOP '%s' (must be a positive integer)\n", argv[2]);
            return -1;
        }
    }

    {
        size_t max_size = (size_t)-1;

        for (i = 0; i < ARRAY_SIZE (offs); i++) {
            size_t off_max;

#if defined(MEMSET)
            off_max = mdst_size - (size_t)offs[i];
#elif defined(MEMCPY)
            off_max = mdst_size - (size_t)offs[i];
            if (msrc_size < off_max)
                off_max = msrc_size;
#elif defined(MEMMOVE)
            off_max = mdst_size - (128U + (size_t)offs[i]);
#endif
            if (off_max < max_size)
                max_size = off_max;
        }

        if (size > max_size) {
            fprintf(stderr,
                    "SIZE %lu too large for configured offsets (max %lu)\n",
                    size, (unsigned long)max_size);
            return -1;
        }
    }

    for (i = 0; i < ARRAY_SIZE (offs); i++) {
        struct timeval start, stop;
        unsigned long elapsed;
        double speed;
        unsigned long j;

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
        if (elapsed == 0)
            elapsed = 1;
        speed = ((double)size * (double)loop * 1000000.0) /
            ((double)elapsed * 1024.0 * 1024.0);
        printf("%s size %lu src_off 0 dst_off %d speed %5.3f MB/s\n",
               argv[0], size, offs[i], speed);
    }

    return 0;
}