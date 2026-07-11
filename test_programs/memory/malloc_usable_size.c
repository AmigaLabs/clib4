/*
 * Test program for malloc_usable_size()
 *
 * Verifies that malloc_usable_size() returns a value >= the requested
 * allocation size for various allocation sizes and patterns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

static int failed = 0;

#define CHECK(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "FAIL: " fmt "\n", ##__VA_ARGS__); \
            failed++; \
        } else { \
            printf("PASS: " fmt "\n", ##__VA_ARGS__); \
        } \
    } while (0)

static void
test_basic_sizes(void) {
    static const size_t sizes[] = { 1, 7, 8, 15, 16, 32, 63, 64, 128, 255,
                                    256, 512, 1024, 4096, 65536, 1048576 };
    size_t n = sizeof(sizes) / sizeof(sizes[0]);

    printf("\n--- Basic size tests ---\n");
    for (size_t i = 0; i < n; i++) {
        size_t req = sizes[i];
        void *p = malloc(req);
        CHECK(p != NULL, "malloc(%zu) succeeded", req);
        if (!p) continue;

        size_t usable = malloc_usable_size(p);
        CHECK(usable >= req,
              "malloc_usable_size for req=%zu => usable=%zu (>= req)",
              req, usable);

        /* Make sure we can actually write usable bytes without crashing. */
        memset(p, 0xAB, usable);

        free(p);
    }
}

static void
test_null_ptr(void) {
    printf("\n--- NULL pointer test ---\n");
    size_t usable = malloc_usable_size(NULL);
    CHECK(usable == 0, "malloc_usable_size(NULL) == 0 (got %zu)", usable);
}

static void
test_realloc(void) {
    printf("\n--- realloc test ---\n");
    void *p = malloc(64);
    CHECK(p != NULL, "malloc(64) succeeded");
    if (!p) return;

    size_t before = malloc_usable_size(p);
    CHECK(before >= 64, "usable before realloc >= 64 (got %zu)", before);

    p = realloc(p, 256);
    CHECK(p != NULL, "realloc(p, 256) succeeded");
    if (!p) return;

    size_t after = malloc_usable_size(p);
    CHECK(after >= 256, "usable after realloc(256) >= 256 (got %zu)", after);

    free(p);
}

static void
test_calloc(void) {
    printf("\n--- calloc test ---\n");
    void *p = calloc(10, 32);
    CHECK(p != NULL, "calloc(10, 32) succeeded");
    if (!p) return;

    size_t usable = malloc_usable_size(p);
    CHECK(usable >= 10 * 32,
          "malloc_usable_size after calloc(10,32) >= 320 (got %zu)", usable);
    free(p);
}

static void
test_aligned_alloc(void) {
    printf("\n--- aligned_alloc test ---\n");
    static const size_t alignments[] = { 16, 32, 64, 128 };
    for (size_t i = 0; i < sizeof(alignments)/sizeof(alignments[0]); i++) {
        size_t align = alignments[i];
        void *p = aligned_alloc(align, 256);
        CHECK(p != NULL, "aligned_alloc(%zu, 256) succeeded", align);
        if (!p) continue;

        CHECK(((uintptr_t)p & (align - 1)) == 0,
              "aligned_alloc(%zu) ptr is properly aligned", align);

        size_t usable = malloc_usable_size(p);
        CHECK(usable >= 256,
              "malloc_usable_size after aligned_alloc(%zu,256) >= 256 (got %zu)",
              align, usable);
        free(p);
    }
}

static void
test_many_allocs(void) {
    printf("\n--- Many simultaneous allocations ---\n");
    enum { N = 128 };
    void *ptrs[N];
    size_t req_sizes[N];

    for (int i = 0; i < N; i++) {
        req_sizes[i] = (size_t)(i + 1) * 17;
        ptrs[i] = malloc(req_sizes[i]);
    }

    int all_ok = 1;
    for (int i = 0; i < N; i++) {
        if (!ptrs[i]) { all_ok = 0; continue; }
        size_t usable = malloc_usable_size(ptrs[i]);
        if (usable < req_sizes[i]) {
            fprintf(stderr, "FAIL: alloc[%d] req=%zu usable=%zu\n",
                    i, req_sizes[i], usable);
            all_ok = 0;
            failed++;
        }
    }
    if (all_ok)
        printf("PASS: all %d allocations have usable >= requested\n", N);

    for (int i = 0; i < N; i++)
        if (ptrs[i]) free(ptrs[i]);
}

int
main(void) {
    printf("=== malloc_usable_size() test ===\n");

    test_null_ptr();
    test_basic_sizes();
    test_realloc();
    test_calloc();
    test_aligned_alloc();
    test_many_allocs();

    printf("\n=== Result: %s ===\n", failed ? "FAILED" : "PASSED");
    return failed ? 1 : 0;
}
