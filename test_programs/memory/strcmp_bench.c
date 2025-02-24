/*
 * String Comparison Benchmarks
 * ============================
 *
 * - Various strcmp methods
 * - Using RTDSC timer
 * - -msse didn't show any diff on platforms 1 and 2
 *
 * Platforms
 * ---------
 *
 * 1.
 * Linux Ubuntu 18 - Intel(R) Core(TM) i7-8550U CPU @ 1.8GHz
 * gcc bench_strcmp.c -O3 (GCC 7.3.0)
 *
 * 2.
 * MacOSX 10.14 - Intel(R) Core(TM) i5-5257U @ 2.70GHz
 * clang bench_strcmp.c -O3 (Clang 1000.11.45)
 *
 * 3.
 * Linux Ubuntu 16 - Intel(R) Core(TM) i7-6700K CPU @ 4.00GHz
 * gcc bench_strcmp.c -DBENCH_TO_RUN=BENCH_<TEST> -O3 (GCC 5.4.0)
 * clang bench_strcmp.c -DBENCH_TO_RUN=BENCH_<TEST> -O3 (Clang 7.0.0)
 *
 * Results
 * -------
 *
 * _Note:_ Picked best times
 *
 *  Platform | strcmp | strcmp with prefix | hash runtime | hash ahead of time
 * ==========|========|====================|==============|===================
 *  1(GCC)   | 1318   | 359                | 1838         | 140
 *  2(Clang) | 32715  | 475                | 2640         | 243
 *  3(GCC)   | 4666   | 2151               | 12923        | 542
 *  3(Clang) | 4592   | 1994               | 13449        | 509
 *
 */

#define BENCH_STRCMP 1
#define BENCH_STRCMP_PREFIX 2
#define BENCH_HASH_RT 3
#define BENCH_HASH_AT 4

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifndef __amigaos4__
#include <x86intrin.h>
#else
#include <unistd.h>

#ifndef CLIB4
static uint64_t rdtsc()
{
    uint32_t high1, high2, low;

    __asm__ __volatile__
    (
    "1: mftbu %0\n"
    "mftb  %1\n"
    "mftbu %2\n"
    "cmpw  %3,%4\n"
    "bne-  1b\n"
    : "=r" (high1), "=r" (low), "=r" (high2)
    : "0" (high1), "2" (high2)
    );

    return ((uint64_t)(high1) << 32) | low;
}
#endif
#endif

const char *strings[] = {
        "a", "b", "c", "1", "2", "3", "abc", "123",
        "if", "else if", "else", "break", "continue", "for", "while", "do",
        "goto", "struct", "int", "float", "unsigned", "double", "char", "const",
        "cpu", "gpu", "memory", "keyboard", "screen", "mouse", "template",
        "compiler", "type", "class", "jaffa cake", "then", "reduce", "reuse",
        "recycle", "black cats", "kiteboard", "surfboard", "skateboard",
        "wakeboard", "wobbleboard", "breadboard",
        "A really long string that takes up space",
        "This is also a longer string that takes up space, time, and sugar",
        "Everybody jump jump! Everybody jump jump jump jump jump jump!",
        "Flowers with purple spots, bannanas and apples",
        "The Quick Brown Fox Jumped Over The Lazy Dog",
        "Lorim Ipsum",
        "Foo", "Bar", "Baz", "Bin", "Fin", "Fab", "Boo", "Far", "FooBar",
        "FooBoo", "BarBar", "Faz", "FinFar", "FabFin", "BazBoo", "BarFoo",
        "a", "b", "c", "1", "2", "3", "abc", "123",
        "if", "else if", "else", "break", "continue", "for", "while", "do",
        "goto", "struct", "int", "float", "unsigned", "double", "char", "const",
        "cpu", "gpu", "memory", "keyboard", "screen", "mouse", "template",
        "compiler", "type", "class", "jaffa cake", "then", "reduce", "reuse",
        "recycle", "black cats", "kiteboard", "surfboard", "skateboard",
        "wakeboard", "wobbleboard", "breadboard",
        "A really long string that takes up space",
        "This is also a longer string that takes up space, time, and sugar",
        "Everybody jump jump! Everybody jump jump jump jump jump jump!",
        "Flowers with purple spots, bannanas and apples",
        "The Quick Brown Fox Jumped Over The Lazy Dog",
        "Lorim Ipsum",
        "Foo", "Bar", "Baz", "Bin", "Fin", "Fab", "Boo", "Far", "FooBar",
        "FooBoo", "BarBar", "Faz", "FinFar", "FabFin", "BazBoo", "BarFoo",
        "a", "b", "c", "1", "2", "3", "abc", "123",
        "if", "else if", "else", "break", "continue", "for", "while", "do",
        "goto", "struct", "int", "float", "unsigned", "double", "char", "const",
        "cpu", "gpu", "memory", "keyboard", "screen", "mouse", "template",
        "compiler", "type", "class", "jaffa cake", "then", "reduce", "reuse",
        "recycle", "black cats", "kiteboard", "surfboard", "skateboard",
        "wakeboard", "wobbleboard", "breadboard",
        "A really long string that takes up space",
        "This is also a longer string that takes up space, time, and sugar",
        "Everybody jump jump! Everybody jump jump jump jump jump jump!",
        "Flowers with purple spots, bannanas and apples",
        "The Quick Brown Fox Jumped Over The Lazy Dog",
        "Lorim Ipsum",
        "Foo", "Bar", "Baz", "Bin", "Fin", "Fab", "Boo", "Far", "FooBar",
        "FooBoo", "BarBar", "Faz", "FinFar", "FabFin", "BazBoo", "BarFoo",
        "a", "b", "c", "1", "2", "3", "abc", "123",
        "if", "else if", "else", "break", "continue", "for", "while", "do",
        "goto", "struct", "int", "float", "unsigned", "double", "char", "const",
        "cpu", "gpu", "memory", "keyboard", "screen", "mouse", "template",
        "compiler", "type", "class", "jaffa cake", "then", "reduce", "reuse",
        "recycle", "black cats", "kiteboard", "surfboard", "skateboard",
        "wakeboard", "wobbleboard", "breadboard",
        "A really long string that takes up space",
        "This is also a longer string that takes up space, time, and sugar",
        "Everybody jump jump! Everybody jump jump jump jump jump jump!",
        "Flowers with purple spots, bannanas and apples",
        "The Quick Brown Fox Jumped Over The Lazy Dog",
        "Lorim Ipsum",
        "Foo", "Bar", "Baz", "Bin", "Fin", "Fab", "Boo", "Far", "FooBar",
        "FooBoo", "BarBar", "Faz", "FinFar", "FabFin", "BazBoo", "BarFoo",
        "a", "b", "c", "1", "2", "3", "abc", "123",
        "if", "else if", "else", "break", "continue", "for", "while", "do",
        "goto", "struct", "int", "float", "unsigned", "double", "char", "const",
        "cpu", "gpu", "memory", "keyboard", "screen", "mouse", "template",
        "compiler", "type", "class", "jaffa cake", "then", "reduce", "reuse",
        "recycle", "black cats", "kiteboard", "surfboard", "skateboard",
        "wakeboard", "wobbleboard", "breadboard",
        "A really long string that takes up space",
        "This is also a longer string that takes up space, time, and sugar",
        "Everybody jump jump! Everybody jump jump jump jump jump jump!",
        "Flowers with purple spots, bannanas and apples",
        "The Quick Brown Fox Jumped Over The Lazy Dog",
        "Lorim Ipsum",
        "Foo", "Bar", "Baz", "Bin", "Fin", "Fab", "Boo", "Far", "FooBar",
        "FooBoo", "BarBar", "Faz", "FinFar", "FabFin", "BazBoo", "BarFoo",
        "a", "b", "c", "1", "2", "3", "abc", "123",
        "if", "else if", "else", "break", "continue", "for", "while", "do",
        "goto", "struct", "int", "float", "unsigned", "double", "char", "const",
        "cpu", "gpu", "memory", "keyboard", "screen", "mouse", "template",
        "compiler", "type", "class", "jaffa cake", "then", "reduce", "reuse",
        "recycle", "black cats", "kiteboard", "surfboard", "skateboard",
        "wakeboard", "wobbleboard", "breadboard",
        "A really long string that takes up space",
        "This is also a longer string that takes up space, time, and sugar",
        "Everybody jump jump! Everybody jump jump jump jump jump jump!",
        "Flowers with purple spots, bannanas and apples",
        "The Quick Brown Fox Jumped Over The Lazy Dog",
        "Lorim Ipsum",
        "Foo", "Bar", "Baz", "Bin", "Fin", "Fab", "Boo", "Far", "FooBar",
        "FooBoo", "BarBar", "Faz", "FinFar", "FabFin", "BazBoo", "BarFoo",

        "needle",

        NULL
};

const char *search_for = "needle";

uint64_t
hash_str(const char *str) {
    uint64_t hash = 5381;
    int c;
    while (c = *str++, c) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}


uint64_t
get_time_rdtsc() {
#ifndef __amigaos4__
    return __builtin_ia32_rdtsc();
#else
    return rdtsc();
#endif
}


/* A basic string compare function */
uint64_t
bench_strcmp() {
    const char **str_it = &strings[0];
    uint64_t start = get_time_rdtsc();

    while (*str_it) {
        if (strcmp(*str_it, search_for) == 0) {
            break;
        }
        ++str_it;
    }

    uint64_t end = get_time_rdtsc();

    printf("Found %s\n", *str_it);

    return end - start;
}


/* Same as above but check the `char` before
   running the strcmp */
int
pre_check(uint8_t *a, uint8_t *b) {
    return *a == *b;
}


uint64_t
bench_strcmp_prefix() {
    const char **str_it = &strings[0];
    uint64_t start = get_time_rdtsc();

    uint8_t *search_int = (uint8_t *) search_for;

    const char *str;
    while (str = *str_it++, str) {
        /* check prefix first */
        if (!pre_check((uint8_t * ) * str_it, search_int)) {
            continue;
        }

        /* compare whole string */
        if (strcmp(*str_it, search_for) == 0) {
            break;
        }
    }

    uint64_t end = get_time_rdtsc();

    printf("Found %s\n", *str_it);

    return end - start;
}


/* hashing strings as we go */
uint64_t
bench_hash_rt() {
    const char **str_it = &strings[0];
    uint64_t start = get_time_rdtsc();
    uint64_t search_hash = hash_str(search_for);

    while (*str_it) {
        uint64_t hash = hash_str(*str_it);
        if (hash == search_hash) {
            break;
        }
        ++str_it;
    }

    uint64_t end = get_time_rdtsc();

    printf("Found %s\n", *str_it);

    return end - start;
}


/* hashing everything ahead of time */
uint64_t
bench_hash_at() {
    /* build hash table */
    uint64_t hash_arr[(sizeof(strings) / sizeof(strings[0]))];
    int count = (sizeof(strings) / sizeof(strings[0]));
    int i;
    for (i = 0; i < count; ++i) {
        if (strings[i] == NULL) {
            hash_arr[i] = (uint64_t) - 1;
        } else {
            hash_arr[i] = hash_str(strings[i]);
        }
    }

    /* search */
    uint64_t *hash_it = &hash_arr[0];
    uint64_t search_hash = hash_str(search_for);

    uint64_t start = get_time_rdtsc();

    while (*hash_it != (uint64_t) - 1) {
        if (*hash_it == search_hash) {
            break;
        }

        ++hash_it;
    }

    uint64_t end = get_time_rdtsc();

    printf("Found %s\n", strings[hash_it - &hash_arr[0]]);

    return end - start;
}


/* Benchmark */
int
main() {
#ifdef CLIB4
    disableOptimizedFunctions();
#endif
    printf("strcmp: %llu\n--\n", bench_strcmp());
    printf("strcmp with prefix: %llu\n--\n", bench_strcmp_prefix());
    printf("hash rt: %llu\n--\n", bench_hash_rt());
    printf("hash at: %llu\n--\n", bench_hash_at());
#ifdef CLIB4
    enableOptimizedFunctions();

    printf("optimized strcmp: %llu\n--\n", bench_strcmp());
    printf("optimized strcmp with prefix: %llu\n--\n", bench_strcmp_prefix());
    printf("optimized hash rt: %llu\n--\n", bench_hash_rt());
    printf("optimized hash at: %llu\n--\n", bench_hash_at());
#endif
    return 0;
}