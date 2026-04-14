/*
 * Test suite for mlock(), munlock(), mlockall(), munlockall() in clib4.
 *
 * Tests cover:
 *  - mlock() / munlock() EINVAL error cases (address+length overflow)
 *  - mlock() / munlock() success cases (zero length, heap, stack, mmap)
 *  - mlockall() EINVAL error cases (unknown flags, MCL_ONFAULT alone)
 *  - mlockall() success cases (MCL_CURRENT, MCL_FUTURE, combination)
 *  - munlockall() always succeeds on AmigaOS 4
 *  - MCL_* constant values as defined in POSIX
 *
 * AmigaOS 4 notes:
 *  - There is no virtual memory / swap subsystem, so all memory is
 *    always physically resident in RAM.
 *  - mlock() / munlock() delegate to IExec->LockMem / IExec->UnlockMem,
 *    which pin memory for hardware DMA stability.
 *  - mlockall() and munlockall() are validated no-ops (all memory is
 *    already resident).
 */

#include "test_framework.h"
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>

/* Page size used by AmigaOS 4 MMU */
#define PAGE_SIZE 4096UL

/* ===================================================================
 * mlock() error-path tests
 * =================================================================== */

/* Test 1: mlock with length zero → success (no-op) */
static const char *test_mlock_zero_len(void) {
    char buf[64] = {0};
    errno = 0;
    int ret = mlock(buf, 0);
    TEST_ASSERT_EQUAL("mlock with len=0 should return 0", 0, ret);
    TEST_ASSERT_EQUAL("errno should be 0 after mlock(len=0)", 0, errno);
    return NULL;
}

/* Test 2: mlock with address+length overflow → EINVAL */
static const char *test_mlock_overflow(void) {
    /* addr = (void*)1, len = SIZE_MAX → 1+SIZE_MAX wraps around */
    void *addr = (void *)(uintptr_t)1;
    errno = 0;
    int ret = mlock(addr, SIZE_MAX);
    TEST_ASSERT_EQUAL("mlock with overflow should return -1", -1, ret);
    TEST_ASSERT_EQUAL("errno should be EINVAL for overflow", EINVAL, errno);
    return NULL;
}

/* ===================================================================
 * munlock() error-path tests
 * =================================================================== */

/* Test 3: munlock with length zero → success (no-op) */
static const char *test_munlock_zero_len(void) {
    char buf[64] = {0};
    errno = 0;
    int ret = munlock(buf, 0);
    TEST_ASSERT_EQUAL("munlock with len=0 should return 0", 0, ret);
    TEST_ASSERT_EQUAL("errno should be 0 after munlock(len=0)", 0, errno);
    return NULL;
}

/* Test 4: munlock with address+length overflow → EINVAL */
static const char *test_munlock_overflow(void) {
    void *addr = (void *)(uintptr_t)1;
    errno = 0;
    int ret = munlock(addr, SIZE_MAX);
    TEST_ASSERT_EQUAL("munlock with overflow should return -1", -1, ret);
    TEST_ASSERT_EQUAL("errno should be EINVAL for overflow", EINVAL, errno);
    return NULL;
}

/* ===================================================================
 * mlock() / munlock() success tests
 * =================================================================== */

/* Test 5: mlock / munlock round-trip on heap memory */
static const char *test_mlock_heap_roundtrip(void) {
    void *buf = malloc(PAGE_SIZE);
    TEST_ASSERT("malloc should succeed", buf != NULL);

    memset(buf, 0xAB, PAGE_SIZE);

    errno = 0;
    int r1 = mlock(buf, PAGE_SIZE);
    TEST_ASSERT_EQUAL("mlock on heap should return 0", 0, r1);
    TEST_ASSERT_EQUAL("errno should be 0 after mlock", 0, errno);

    /* Data must be intact after lock */
    TEST_ASSERT_EQUAL("data byte 0 should be 0xAB after mlock",
                      (unsigned char)0xAB, ((unsigned char *)buf)[0]);
    TEST_ASSERT_EQUAL("data byte N-1 should be 0xAB after mlock",
                      (unsigned char)0xAB, ((unsigned char *)buf)[PAGE_SIZE - 1]);

    errno = 0;
    int r2 = munlock(buf, PAGE_SIZE);
    TEST_ASSERT_EQUAL("munlock on heap should return 0", 0, r2);
    TEST_ASSERT_EQUAL("errno should be 0 after munlock", 0, errno);

    free(buf);
    return NULL;
}

/* Test 6: mlock / munlock on page-aligned heap memory */
static const char *test_mlock_page_aligned(void) {
    void *buf = memalign(PAGE_SIZE, PAGE_SIZE * 2);
    TEST_ASSERT("memalign should succeed", buf != NULL);

    int r1 = mlock(buf, PAGE_SIZE * 2);
    TEST_ASSERT_EQUAL("mlock on page-aligned heap should return 0", 0, r1);

    int r2 = munlock(buf, PAGE_SIZE * 2);
    TEST_ASSERT_EQUAL("munlock on page-aligned heap should return 0", 0, r2);

    free(buf);
    return NULL;
}

/* Test 7: mlock a stack-allocated buffer */
static const char *test_mlock_stack(void) {
    /* Use a local array large enough to span at least one page */
    unsigned char stack_buf[PAGE_SIZE * 2];
    memset(stack_buf, 0x5A, sizeof(stack_buf));

    errno = 0;
    int r1 = mlock(stack_buf, sizeof(stack_buf));
    TEST_ASSERT_EQUAL("mlock on stack should return 0", 0, r1);

    /* Data must remain intact */
    TEST_ASSERT_EQUAL("stack data should be 0x5A after mlock",
                      (unsigned char)0x5A, stack_buf[0]);

    int r2 = munlock(stack_buf, sizeof(stack_buf));
    TEST_ASSERT_EQUAL("munlock on stack should return 0", 0, r2);

    return NULL;
}

/* Test 8: mlock / munlock on an anonymous mmap'd region */
static const char *test_mlock_mmap_anonymous(void) {
    void *ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    TEST_ASSERT("mmap should succeed", ptr != MAP_FAILED);

    /* Write a pattern */
    memset(ptr, 0xCC, PAGE_SIZE);

    int r1 = mlock(ptr, PAGE_SIZE);
    TEST_ASSERT_EQUAL("mlock on mmap region should return 0", 0, r1);

    /* Data must survive the lock */
    TEST_ASSERT_EQUAL("mmap data should be 0xCC after mlock",
                      (unsigned char)0xCC, ((unsigned char *)ptr)[0]);

    int r2 = munlock(ptr, PAGE_SIZE);
    TEST_ASSERT_EQUAL("munlock on mmap region should return 0", 0, r2);

    munmap(ptr, PAGE_SIZE);
    return NULL;
}

/* Test 9: multiple separate mlock / munlock calls, no double-unlock issues */
static const char *test_mlock_multiple_regions(void) {
    void *a = malloc(PAGE_SIZE);
    void *b = malloc(PAGE_SIZE);
    TEST_ASSERT("malloc a should succeed", a != NULL);
    TEST_ASSERT("malloc b should succeed", b != NULL);

    TEST_ASSERT_EQUAL("mlock a should succeed", 0, mlock(a, PAGE_SIZE));
    TEST_ASSERT_EQUAL("mlock b should succeed", 0, mlock(b, PAGE_SIZE));

    TEST_ASSERT_EQUAL("munlock a should succeed", 0, munlock(a, PAGE_SIZE));
    TEST_ASSERT_EQUAL("munlock b should succeed", 0, munlock(b, PAGE_SIZE));

    free(a);
    free(b);
    return NULL;
}

/* ===================================================================
 * mlockall() error-path tests
 * =================================================================== */

/* Test 10: mlockall with unknown (invalid) flag → EINVAL */
static const char *test_mlockall_invalid_flags(void) {
    int bad_flags = 0x80;   /* Not MCL_CURRENT, MCL_FUTURE, or MCL_ONFAULT */
    errno = 0;
    int ret = mlockall(bad_flags);
    TEST_ASSERT_EQUAL("mlockall with unknown flags should return -1", -1, ret);
    TEST_ASSERT_EQUAL("errno should be EINVAL for unknown flags", EINVAL, errno);
    return NULL;
}

/* Test 11: mlockall with MCL_ONFAULT alone (no MCL_CURRENT/FUTURE) → EINVAL */
static const char *test_mlockall_onfault_alone(void) {
    errno = 0;
    int ret = mlockall(MCL_ONFAULT);
    TEST_ASSERT_EQUAL("mlockall(MCL_ONFAULT) alone should return -1", -1, ret);
    TEST_ASSERT_EQUAL("errno should be EINVAL for MCL_ONFAULT alone", EINVAL, errno);
    return NULL;
}

/* ===================================================================
 * mlockall() success tests
 * =================================================================== */

/* Test 12: mlockall(MCL_CURRENT) → 0 */
static const char *test_mlockall_current(void) {
    errno = 0;
    int ret = mlockall(MCL_CURRENT);
    TEST_ASSERT_EQUAL("mlockall(MCL_CURRENT) should return 0", 0, ret);
    TEST_ASSERT_EQUAL("errno should be 0", 0, errno);
    return NULL;
}

/* Test 13: mlockall(MCL_FUTURE) → 0 */
static const char *test_mlockall_future(void) {
    errno = 0;
    int ret = mlockall(MCL_FUTURE);
    TEST_ASSERT_EQUAL("mlockall(MCL_FUTURE) should return 0", 0, ret);
    TEST_ASSERT_EQUAL("errno should be 0", 0, errno);
    return NULL;
}

/* Test 14: mlockall(MCL_CURRENT | MCL_FUTURE) → 0 */
static const char *test_mlockall_current_future(void) {
    errno = 0;
    int ret = mlockall(MCL_CURRENT | MCL_FUTURE);
    TEST_ASSERT_EQUAL("mlockall(MCL_CURRENT|MCL_FUTURE) should return 0", 0, ret);
    TEST_ASSERT_EQUAL("errno should be 0", 0, errno);
    return NULL;
}

/* Test 15: mlockall(MCL_CURRENT | MCL_ONFAULT) → 0 */
static const char *test_mlockall_current_onfault(void) {
    errno = 0;
    int ret = mlockall(MCL_CURRENT | MCL_ONFAULT);
    TEST_ASSERT_EQUAL("mlockall(MCL_CURRENT|MCL_ONFAULT) should return 0",
                      0, ret);
    TEST_ASSERT_EQUAL("errno should be 0", 0, errno);
    return NULL;
}

/* ===================================================================
 * munlockall() tests
 * =================================================================== */

/* Test 16: munlockall → always 0 */
static const char *test_munlockall(void) {
    errno = 0;
    int ret = munlockall();
    TEST_ASSERT_EQUAL("munlockall() should return 0", 0, ret);
    TEST_ASSERT_EQUAL("errno should be 0 after munlockall", 0, errno);
    return NULL;
}

/* ===================================================================
 * MCL_* constant value tests
 * =================================================================== */

/* Test 17: MCL_* constants have correct POSIX values */
static const char *test_mcl_constants(void) {
    TEST_ASSERT_EQUAL("MCL_CURRENT should be 1", 1, MCL_CURRENT);
    TEST_ASSERT_EQUAL("MCL_FUTURE should be 2",  2, MCL_FUTURE);
    TEST_ASSERT_EQUAL("MCL_ONFAULT should be 4", 4, MCL_ONFAULT);
    return NULL;
}

/* ===================================================================
 * mlock2() tests
 * =================================================================== */

/* Test 18: mlock2 with flags=0 behaves like mlock() */
static const char *test_mlock2_flags_zero(void) {
    void *buf = malloc(PAGE_SIZE);
    TEST_ASSERT("malloc should succeed", buf != NULL);

    errno = 0;
    int r1 = mlock2(buf, PAGE_SIZE, 0);
    TEST_ASSERT_EQUAL("mlock2(flags=0) should return 0", 0, r1);
    TEST_ASSERT_EQUAL("errno should be 0 after mlock2(flags=0)", 0, errno);

    int r2 = munlock(buf, PAGE_SIZE);
    TEST_ASSERT_EQUAL("munlock after mlock2 should return 0", 0, r2);

    free(buf);
    return NULL;
}

/* Test 19: mlock2 with MLOCK_ONFAULT */
static const char *test_mlock2_onfault(void) {
    void *buf = malloc(PAGE_SIZE);
    TEST_ASSERT("malloc should succeed", buf != NULL);

    errno = 0;
    int r1 = mlock2(buf, PAGE_SIZE, MLOCK_ONFAULT);
    TEST_ASSERT_EQUAL("mlock2(MLOCK_ONFAULT) should return 0", 0, r1);
    TEST_ASSERT_EQUAL("errno should be 0 after mlock2(MLOCK_ONFAULT)", 0, errno);

    int r2 = munlock(buf, PAGE_SIZE);
    TEST_ASSERT_EQUAL("munlock after mlock2 should return 0", 0, r2);

    free(buf);
    return NULL;
}

/* Test 20: mlock2 with unknown flags → EINVAL */
static const char *test_mlock2_invalid_flags(void) {
    void *buf = malloc(PAGE_SIZE);
    TEST_ASSERT("malloc should succeed", buf != NULL);

    errno = 0;
    int ret = mlock2(buf, PAGE_SIZE, 0xFF);
    int saved_errno = errno;
    free(buf);

    TEST_ASSERT_EQUAL("mlock2 with unknown flags should return -1", -1, ret);
    TEST_ASSERT_EQUAL("errno should be EINVAL for unknown flags", EINVAL, saved_errno);
    return NULL;
}

/* Test 21: mlock2 with len=0 → 0 (no-op) regardless of flags */
static const char *test_mlock2_zero_len(void) {
    char buf[64] = {0};
    errno = 0;
    int ret = mlock2(buf, 0, MLOCK_ONFAULT);
    TEST_ASSERT_EQUAL("mlock2 with len=0 should return 0", 0, ret);
    TEST_ASSERT_EQUAL("errno should be 0 after mlock2(len=0)", 0, errno);
    return NULL;
}

/* Test 22: mlock2 with address+length overflow → EINVAL */
static const char *test_mlock2_overflow(void) {
    void *addr = (void *)(uintptr_t)1;
    errno = 0;
    int ret = mlock2(addr, SIZE_MAX, 0);
    TEST_ASSERT_EQUAL("mlock2 with overflow should return -1", -1, ret);
    TEST_ASSERT_EQUAL("errno should be EINVAL for overflow", EINVAL, errno);
    return NULL;
}

/* Test 23: MLOCK_ONFAULT constant has correct value */
static const char *test_mlock_onfault_constant(void) {
    TEST_ASSERT_EQUAL("MLOCK_ONFAULT should be 1", 1, (int)MLOCK_ONFAULT);
    return NULL;
}

/* ===================================================================
 * Test runner
 * =================================================================== */

#define RUN_TEST(fn) do { \
    const char *result = fn(); \
    if (result != NULL) { \
        printf(COLOR_RED "   FAILED: %s\n" COLOR_RESET, result); \
    } \
} while (0)

int main(void) {
    printf("\n=== mlock / munlock / mlock2 / mlockall / munlockall tests ===\n\n");
    printf("INFO: On AmigaOS 4, mlock/mlock2/munlock use IExec->LockMem/UnlockMem.\n");
    printf("INFO: mlockall/munlockall are no-ops (no swap exists).\n\n");

    /* mlock error paths */
    printf("--- mlock() error paths ---\n");
    RUN_TEST(test_mlock_zero_len);
    RUN_TEST(test_mlock_overflow);

    /* munlock error paths */
    printf("\n--- munlock() error paths ---\n");
    RUN_TEST(test_munlock_zero_len);
    RUN_TEST(test_munlock_overflow);

    /* mlock/munlock success */
    printf("\n--- mlock() / munlock() success ---\n");
    RUN_TEST(test_mlock_heap_roundtrip);
    RUN_TEST(test_mlock_page_aligned);
    RUN_TEST(test_mlock_stack);
    RUN_TEST(test_mlock_mmap_anonymous);
    RUN_TEST(test_mlock_multiple_regions);

    /* mlockall error paths */
    printf("\n--- mlockall() error paths ---\n");
    RUN_TEST(test_mlockall_invalid_flags);
    RUN_TEST(test_mlockall_onfault_alone);

    /* mlockall success */
    printf("\n--- mlockall() success ---\n");
    RUN_TEST(test_mlockall_current);
    RUN_TEST(test_mlockall_future);
    RUN_TEST(test_mlockall_current_future);
    RUN_TEST(test_mlockall_current_onfault);

    /* munlockall */
    printf("\n--- munlockall() ---\n");
    RUN_TEST(test_munlockall);

    /* Constants */
    printf("\n--- MCL_* constants ---\n");
    RUN_TEST(test_mcl_constants);

    /* mlock2 */
    printf("\n--- mlock2() ---\n");
    RUN_TEST(test_mlock2_flags_zero);
    RUN_TEST(test_mlock2_onfault);
    RUN_TEST(test_mlock2_invalid_flags);
    RUN_TEST(test_mlock2_zero_len);
    RUN_TEST(test_mlock2_overflow);
    RUN_TEST(test_mlock_onfault_constant);

    /* Summary */
    printf("\n=== Results: %d/%d tests passed ===\n\n",
           tests_passed, tests_run);

    if (tests_failed > 0) {
        printf(COLOR_RED "FAILED: %d test(s) failed\n" COLOR_RESET,
               tests_failed);
        return 1;
    }

    printf(COLOR_GREEN "All tests passed!\n" COLOR_RESET);
    return 0;
}
