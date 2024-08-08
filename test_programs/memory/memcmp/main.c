#define _XOPEN_SOURCE 500 // random, srandom

#include <assert.h>  // assert
#include <math.h>    // isfinite, sqrt
#include <stdbool.h> // bool, false, true
#include <stddef.h>  // size_t
#include <stdio.h>   // FILE, fopen, fclose, fprintf, snprintf, stderr
#include <stdlib.h>  // NULL, EXIT_{SUCCESS,ERROR}, malloc, free, random, srandom
#include <string.h>  // memcpy
#include <time.h>    // CLOCKS_PER_SEC, clock_t, clock

#include "memcmp.h"

// Tells the compiler that all previous changes to memory shall be visible.
#define CLOBBER_MEMORY() __asm__ volatile("" \
                                          :  \
                                          :  \
                                          : "memory")

// Tells the compiler that the value of VAR shall be visible.
#define USE_VARIABLE(VAR) __asm__ volatile(""          \
                                           :           \
                                           : "rm"(VAR) \
                                           :)

// Type of the function to benchmark.
typedef bool (*memcmp_type)(const void *, const void *, size_t);

// Unsigned integer type that refers to a machine word.
typedef unsigned long word_type;

// Size of a machine word.
static const size_t word_size = sizeof(word_type);

// Global error message.
static const char *errmsg;

// Average value and statistical error of a statistical data sample.
typedef struct {
    double average; // average value (eg mean)
    double error;   // statistical error
} timing_result;

// Computes the square of a number.
static double
square(const double x) {
    return x * x;
}

// Computes mean and standard deviation of the `n` data points in the array
// `datapoints`.  `n` must be at least 3.  The mean is stored at
// `resultptr->average` and the standard deviation at `resultptr->error`.
// Returns 0 on success.  On error a negative number is returned and `errmsg`
// is set.
static int
do_statistics(const double *const datapoints,
              const size_t n,
              timing_result *const resultptr) {
    assert(n >= 3);
    double accu = 0.0;
    for (size_t i = 0; i < n; ++i)
        accu += datapoints[i];
    const double mean = accu / n;
    if (!isfinite(mean)) {
        errmsg = "non-finite mean";
        return -1;
    }
    accu = 0.0;
    for (size_t i = 0; i < n; ++i)
        accu += square(datapoints[i] - mean);
    const double stdev = sqrt(accu / (n - 1));
    if (!isfinite(stdev)) {
        errmsg = "non-finite standard deviation";
        return -1;
    }
    resultptr->average = mean;
    resultptr->error = stdev;
    return 0;
}

// Randomly maybe changes a single byte in the word pointed to by `wp`.
// Returns whether any byte was changed.
bool maybe_change_byte(word_type *const wp) {
    assert(wp != NULL);
    unsigned char bytes[word_size];
    const word_type before = *wp;
    memcpy(bytes, wp, word_size);
    const size_t index = random() % word_size;
    if ((random() % word_size) != 0)
        bytes[index] = (unsigned char) random();
    memcpy(wp, bytes, word_size);
    return (*wp != before);
}

// Runs a single benchmark for the function `funcptr` on an input of
// approximately `input_size` bytes.  The function will be called one more than
// `repetitions` times and the execution time of all but the first run will be
// collected statistically and stored in the record pointed to by `resultptr`.
// `repetitions` must be at least 3.  `buff1st` and `buff2nd` must be buffers
// of at least `input_size` bytes that can be accessed as arrays of
// `word_type`.  The buffers must not overlap.  They will be used internally as
// work-space.  On success, 0 is returned.  On error, a negative number is
// returned and `errmsg` is set.
static int
run_benchmark(const memcmp_type funcptr,
              const size_t input_size,
              const size_t repetitions,
              word_type *restrict const buff1st,
              word_type *restrict const buff2nd,
              timing_result *const resultptr) {

    const size_t words = input_size / word_size;
    const size_t bytes = words * word_size;
    double timings[repetitions + 1];

    for (size_t i = 0; i < words; ++i) {
        const word_type w = random();
        buff1st[i] = w;
        buff2nd[i] = w;
    }

    for (size_t i = 0; i <= repetitions; ++i) {
        const clock_t no_clock = (clock_t) - 1;
        const bool expected = (words >= 2)
                              ? !maybe_change_byte(buff2nd + words - 2)
                              : true;
        const size_t skip = (words >= 1) ? (random() % word_size) : 0;
        CLOBBER_MEMORY();
        const clock_t t0 = clock();
        const int actual = funcptr((const char *) buff1st + skip,
                                   (const char *) buff2nd + skip,
                                   bytes - skip);
        USE_VARIABLE(actual);
        const clock_t t1 = clock();

        if (actual != expected) {
            errmsg = "function returned wrong result";
            goto label_catch;
        }
        if ((t0 == no_clock) || (t1 == no_clock)) {
            errmsg = "cannot get CPU time";
            goto label_catch;
        }
        timings[i] = (double) (t1 - t0) / CLOCKS_PER_SEC;
        if (words >= 2)
            buff2nd[words - 2] = buff1st[words - 2];
    }
    if (do_statistics(timings + 1, repetitions, resultptr) < 0)
        goto label_catch;

    resultptr->error += 1.0 / CLOCKS_PER_SEC; // systematic error
    goto label_finally;
label_catch:
    assert(errmsg != NULL);
label_finally:
    return (errmsg == NULL) ? 0 : -1;
}

// Runs all benchmarks and for randomly chosen input sizes and writes the
// results to text files named `timing_${tag}.dat` in the current working
// directory where ${tag} identifies the benchmarked function.  Progress
// information is printed to standard error output.  Returns 0 on success.  On
// error, a negative number is returned and `errmsg` is set.
static int
run_all_benchmarks(size_t max_size, size_t repetitions) {
    const size_t _repetitions = 5;
    const size_t datapoints = 50;
    const size_t _max_size = 29 * (1ULL << 20); // \lessapprox 30 MiB
    if (max_size <= 0)
        max_size = _max_size;
    if (repetitions <= 0)
        repetitions = _repetitions;

    const size_t candidates = 3;
    const memcmp_type cand_funcs[] = {memcmp_naive, memcmp_stdlib, memcmp_yamiez};
    const char *const cand_names[] = {"naive", "stdlib", "yamiez"};

    word_type *const buff1st = malloc(max_size);
    word_type *const buff2nd = malloc(max_size);

    if ((buff1st == NULL) || (buff2nd == NULL)) {
        errmsg = "out of memory";
        goto label_catch_outer;
    }
    for (size_t candidx = 0; candidx < candidates; ++candidx) {
        FILE *fh = NULL;
        fprintf(stderr, "%-12s ", cand_names[candidx]);
        char filename[128];
        const int status = snprintf(filename, sizeof(filename), "timing_%s.dat", cand_names[candidx]);
        if ((status < 0) || ((size_t) status >= sizeof(filename))) {
            errmsg = "error in snprintf";
            goto label_catch_inner;
        }
        fh = fopen(filename, "w");
        if (fh == NULL) {
            errmsg = "cannot open output file";
            goto label_catch_inner;
        }
        if (fprintf(fh, "# %22s %24s %24s\n\n", "n", "average / s", "error / s") < 0) {
            errmsg = "I/O error";
            goto label_catch_inner;
        }
        for (size_t j = 0; j < datapoints; ++j) {
            size_t n = random() % max_size;
            timing_result result = {0.0, 0.0};
            if (run_benchmark(cand_funcs[candidx], n, repetitions, buff1st, buff2nd, &result) < 0)
                goto label_catch_inner;
            if (fprintf(fh, "%24zu %24.10e %24.10e\n", n, result.average, result.error) < 0) {
                errmsg = "I/O error";
                goto label_catch_inner;
            }
            fputc('.', stderr);
        }
        goto label_finally_inner;
label_catch_inner:
        assert(errmsg != NULL);
label_finally_inner:
        if (fh != NULL) {
            if (fclose(fh) != 0)
                errmsg = "error closing output file";
        }
        fprintf(stderr, "  %s\n", (errmsg != NULL) ? "failed" : "done");
        if (errmsg != NULL)
            goto label_finally_outer;
    }
    goto label_finally_outer;
label_catch_outer:
    assert(errmsg != NULL);
label_finally_outer:
    free(buff1st);
    free(buff2nd);
    return (errmsg != NULL) ? -1 : 0;
}

// Seeds the global pseudo random number generator via `srandom` and runs all
// benchmarks.  Progress information and error messages are printed to standard
// error output.  Benchmark results are written to text files in the current
// working directory.  Returns `EXIT_SUCCESS` on success or `EXIT_FAILURE` on
// error.
int main(int argc, char **argv) {
    size_t max_size = -1;
    size_t repetitions = -1;

    srandom((unsigned int) clock());
    if (argc > 1) {
        max_size = atoi(argv[1]);
        if (max_size < 10000)
            max_size = 10000;
    }
    if (argc > 2) {
        repetitions = atoi(argv[2]);
        if (repetitions < 10)
            repetitions = 10;
    }

    if (run_all_benchmarks(max_size, repetitions) < 0) {
        if (errmsg == NULL)
            errmsg = "unknown error";
        fprintf(stderr, "error: %s\n", errmsg);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}