#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

static const char *__attribute__((used)) stack = "$STACK: 1553500\0";

typedef struct {
    uint64_t low64;
    uint64_t high64;
} XXH128_hash_t;

__attribute__((__noinline__)) int
XXH128_hash_cmp_ptr(const XXH128_hash_t *a, const XXH128_hash_t *b) {
    if (a->high64 == b->high64) {
        if (a->low64 == b->low64) {
            return 0;
        } else if (a->low64 < b->low64) {
            return -1;
        } else {
            return 1;
        }
    }
    if (a->high64 < b->high64) {
        return -1;
    } else {
        return 1;
    }
}

__attribute__((__noinline__)) int
XXH128_hash_cmp(const XXH128_hash_t a, const XXH128_hash_t b) {
    if (a.high64 == b.high64) {
        if (a.low64 == b.low64) {
            return 0;
        } else if (a.low64 < b.low64) {
            return -1;
        } else {
            return 1;
        }
    }
    if (a.high64 < b.high64) {
        return -1;
    } else {
        return 1;
    }
}

__attribute__((__noinline__)) int
compare128_branchless(XXH128_hash_t a, XXH128_hash_t b) {
    int hbigger = 2 * ((a.high64 > b.high64) - (b.high64 > a.high64));
    int lbigger = (a.low64 > b.low64) - (b.low64 > a.low64);
    return hbigger + lbigger;
}

__attribute__((__always_inline__)) int
compare128_branchless_inline(XXH128_hash_t a, XXH128_hash_t b) {
    int hbigger = 2 * ((a.high64 > b.high64) - (b.high64 > a.high64));
    int lbigger = (a.low64 > b.low64) - (b.low64 > a.low64);
    return hbigger + lbigger;
}

__attribute__((__noinline__)) int
compare128_branchless_ptr(const XXH128_hash_t *a, const XXH128_hash_t *b) {
    int hbigger = 2 * ((a->high64 > b->high64) - (b->high64 > a->high64));
    int lbigger = (a->low64 > b->low64) - (b->low64 > a->low64);
    return hbigger + lbigger;
}


__attribute__((__noinline__)) int
compare128_onebranch(XXH128_hash_t a, XXH128_hash_t b) {
    int const hbigger = (a.high64 > b.high64) - (b.high64 > a.high64);
    if (hbigger) return hbigger;
    return (a.low64 > b.low64) - (b.low64 > a.low64);
}

__attribute__((__always_inline__)) int
compare128_onebranch_inline(XXH128_hash_t a, XXH128_hash_t b) {
    int const hbigger = (a.high64 > b.high64) - (b.high64 > a.high64);
    if (hbigger) return hbigger;
    return (a.low64 > b.low64) - (b.low64 > a.low64);
}

__attribute__((__noinline__)) int
compare128_onebranch_ptr(const XXH128_hash_t *a, const XXH128_hash_t *b) {
    int const hbigger = (a->high64 > b->high64) - (b->high64 > a->high64);
    if (hbigger) return hbigger;
    return (a->low64 > b->low64) - (b->low64 > a->low64);
}


int XXH128_hash_cmp_qsort(const void *start, const void *end) {
    return XXH128_hash_cmp(*(const XXH128_hash_t *) start, *(const XXH128_hash_t *) end);
}

int compare128_branchless_qsort(const void *start, const void *end) {
    return compare128_branchless(*(const XXH128_hash_t *) start, *(const XXH128_hash_t *) end);
}

int compare128_branchless_inline_qsort(const void *start, const void *end) {
    return compare128_branchless_inline(*(const XXH128_hash_t *) start, *(const XXH128_hash_t *) end);
}

int compare128_onebranch_qsort(const void *start, const void *end) {
    return compare128_onebranch(*(const XXH128_hash_t *) start, *(const XXH128_hash_t *) end);
}

int compare128_onebranch_inline_qsort(const void *start, const void *end) {
    return compare128_onebranch_inline(*(const XXH128_hash_t *) start, *(const XXH128_hash_t *) end);
}


#define ROUNDS 2000000 //0
static XXH128_hash_t arr_src[ROUNDS];
static XXH128_hash_t arr_wksp[ROUNDS];

int main() {
    srand(time(NULL));
    printf("arch: %zu-bit\n", sizeof(void *) * 8);
    for (int i = 0; i < ROUNDS; i++) {
        arr_src[i].low64 = rand() | (uint64_t) rand() << 32;
        arr_src[i].high64 = rand() | (uint64_t) rand() << 32;
    }
    double start, end;

    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), XXH128_hash_cmp_qsort);
    end = (double) clock();
    printf("pass by value: %lf\n", (end - start) / CLOCKS_PER_SEC);

    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), compare128_branchless_qsort);
    end = (double) clock();
    printf("pass by value - branchless: %lf\n", (end - start) / CLOCKS_PER_SEC);

    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), compare128_branchless_inline_qsort);
    end = (double) clock();
    printf("pass by value - branchless+inline: %lf\n", (end - start) / CLOCKS_PER_SEC);

    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), compare128_onebranch_qsort);
    end = (double) clock();
    printf("pass by value - one_branch: %lf\n", (end - start) / CLOCKS_PER_SEC);

    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), compare128_onebranch_inline_qsort);
    end = (double) clock();
    printf("pass by value - one_branch+inline: %lf\n", (end - start) / CLOCKS_PER_SEC);


    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), (int (*)(const void *, const void *)) XXH128_hash_cmp_ptr);
    end = (double) clock();
    printf("direct pass to qsort: %lf\n", (end - start) / CLOCKS_PER_SEC);

    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), (int (*)(const void *, const void *)) compare128_branchless_ptr);
    end = (double) clock();
    printf("direct pass to qsort - branchless: %lf\n", (end - start) / CLOCKS_PER_SEC);

    memcpy(arr_wksp, arr_src, sizeof(arr_wksp));
    start = (double) clock();
    qsort(arr_wksp, ROUNDS, sizeof(XXH128_hash_t), (int (*)(const void *, const void *)) compare128_onebranch_ptr);
    end = (double) clock();
    printf("direct pass to qsort - one_branch: %lf\n", (end - start) / CLOCKS_PER_SEC);
}
