/*
 * Simple C Test Framework for clib4
 * Inspired by minunit - minimal unit testing framework
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Test statistics */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Color codes for output */
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_RESET   "\033[0m"

/* Test macros */
#define TEST_ASSERT(message, test) do { \
    tests_run++; \
    if (!(test)) { \
        tests_failed++; \
        printf(COLOR_RED "  x FAIL: %s\n" COLOR_RESET, message); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
        return message; \
    } else { \
        tests_passed++; \
        printf(COLOR_GREEN "  + PASS: %s\n" COLOR_RESET, message); \
    } \
} while (0)

#define TEST_ASSERT_EQUAL(message, expected, actual) do { \
    tests_run++; \
    if ((expected) != (actual)) { \
        tests_failed++; \
        printf(COLOR_RED "  x FAIL: %s\n" COLOR_RESET, message); \
        printf("    Expected: %ld, Got: %ld\n", (long)(expected), (long)(actual)); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
        return message; \
    } else { \
        tests_passed++; \
        printf(COLOR_GREEN "  + PASS: %s\n" COLOR_RESET, message); \
    } \
} while (0)

#define TEST_ASSERT_STR_EQUAL(message, expected, actual) do { \
    tests_run++; \
    if (strcmp((expected), (actual)) != 0) { \
        tests_failed++; \
        printf(COLOR_RED "  x FAIL: %s\n" COLOR_RESET, message); \
        printf("    Expected: \"%s\", Got: \"%s\"\n", expected, actual); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
        return message; \
    } else { \
        tests_passed++; \
        printf(COLOR_GREEN "  + PASS: %s\n" COLOR_RESET, message); \
    } \
} while (0)

#define TEST_ASSERT_MEM_EQUAL(message, expected, actual, size) do { \
    tests_run++; \
    if (memcmp((expected), (actual), (size)) != 0) { \
        tests_failed++; \
        printf(COLOR_RED "  x FAIL: %s\n" COLOR_RESET, message); \
        printf("    Memory contents differ\n"); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
        return message; \
    } else { \
        tests_passed++; \
        printf(COLOR_GREEN "  + PASS: %s\n" COLOR_RESET, message); \
    } \
} while (0)

#define TEST_ASSERT_NULL(message, ptr) do { \
    tests_run++; \
    if ((ptr) != NULL) { \
        tests_failed++; \
        printf(COLOR_RED "  x FAIL: %s\n" COLOR_RESET, message); \
        printf("    Expected NULL, got %p\n", (void*)(ptr)); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
        return message; \
    } else { \
        tests_passed++; \
        printf(COLOR_GREEN "  + PASS: %s\n" COLOR_RESET, message); \
    } \
} while (0)

#define TEST_ASSERT_NOT_NULL(message, ptr) do { \
    tests_run++; \
    if ((ptr) == NULL) { \
        tests_failed++; \
        printf(COLOR_RED "  x FAIL: %s\n" COLOR_RESET, message); \
        printf("    Expected non-NULL pointer\n"); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
        return message; \
    } else { \
        tests_passed++; \
        printf(COLOR_GREEN "  + PASS: %s\n" COLOR_RESET, message); \
    } \
} while (0)

#define RUN_TEST(test) do { \
    const char *message = test(); \
    if (message) return message; \
} while (0)

/* Test suite helpers */
#define BEGIN_TEST_SUITE(name) \
    printf("\n" COLOR_YELLOW "Running test suite: %s\n" COLOR_RESET, name); \
    tests_run = 0; \
    tests_passed = 0; \
    tests_failed = 0;

#define END_TEST_SUITE() \
    printf("\n" COLOR_YELLOW "Test suite summary:\n" COLOR_RESET); \
    printf("  Total tests: %d\n", tests_run); \
    printf("  " COLOR_GREEN "Passed: %d\n" COLOR_RESET, tests_passed); \
    if (tests_failed > 0) { \
        printf("  " COLOR_RED "Failed: %d\n" COLOR_RESET, tests_failed); \
    } else { \
        printf("  Failed: 0\n"); \
    } \
    printf("\n");

#endif /* TEST_FRAMEWORK_H */
