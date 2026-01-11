/*
 * Test suite for stdlib.h functions
 */

#include "test_framework.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

/* Test atoi */
static const char *test_atoi(void) {
    TEST_ASSERT_EQUAL("atoi '123'", 123, atoi("123"));
    TEST_ASSERT_EQUAL("atoi '-456'", -456, atoi("-456"));
    TEST_ASSERT_EQUAL("atoi '0'", 0, atoi("0"));
    TEST_ASSERT_EQUAL("atoi with spaces", 42, atoi("  42"));
    TEST_ASSERT_EQUAL("atoi with trailing", 100, atoi("100abc"));
    return NULL;
}

/* Test atol */
static const char *test_atol(void) {
    TEST_ASSERT_EQUAL("atol '123456'", 123456L, atol("123456"));
    TEST_ASSERT_EQUAL("atol '-789'", -789L, atol("-789"));
    TEST_ASSERT_EQUAL("atol '0'", 0L, atol("0"));
    return NULL;
}

/* Test atof */
static const char *test_atof(void) {
    double result;
    
    result = atof("123.45");
    TEST_ASSERT("atof '123.45'", result > 123.44 && result < 123.46);
    
    result = atof("-67.89");
    TEST_ASSERT("atof '-67.89'", result > -67.90 && result < -67.88);
    
    result = atof("0.0");
    TEST_ASSERT_EQUAL("atof '0.0'", 0, (int)result);
    
    return NULL;
}

/* Test strtol */
static const char *test_strtol(void) {
    char *endptr;
    long result;
    
    result = strtol("123", &endptr, 10);
    TEST_ASSERT_EQUAL("strtol '123' base 10", 123, result);
    
    result = strtol("  -456", &endptr, 10);
    TEST_ASSERT_EQUAL("strtol '-456'", -456, result);
    
    result = strtol("0x1A", &endptr, 16);
    TEST_ASSERT_EQUAL("strtol '0x1A' base 16", 26, result);
    
    result = strtol("1010", &endptr, 2);
    TEST_ASSERT_EQUAL("strtol '1010' base 2", 10, result);
    
    result = strtol("777", &endptr, 8);
    TEST_ASSERT_EQUAL("strtol '777' base 8", 511, result);
    
    return NULL;
}

/* Test strtoul */
static const char *test_strtoul(void) {
    char *endptr;
    unsigned long result;
    
    result = strtoul("123", &endptr, 10);
    TEST_ASSERT_EQUAL("strtoul '123'", 123, result);
    
    result = strtoul("0xFF", &endptr, 16);
    TEST_ASSERT_EQUAL("strtoul '0xFF' base 16", 255, result);
    
    result = strtoul("1111", &endptr, 2);
    TEST_ASSERT_EQUAL("strtoul '1111' base 2", 15, result);
    
    return NULL;
}

/* Test abs */
static const char *test_abs(void) {
    TEST_ASSERT_EQUAL("abs(5)", 5, abs(5));
    TEST_ASSERT_EQUAL("abs(-5)", 5, abs(-5));
    TEST_ASSERT_EQUAL("abs(0)", 0, abs(0));
    TEST_ASSERT_EQUAL("abs(INT_MAX)", INT_MAX, abs(INT_MAX));
    return NULL;
}

/* Test labs */
static const char *test_labs(void) {
    TEST_ASSERT_EQUAL("labs(100)", 100, labs(100));
    TEST_ASSERT_EQUAL("labs(-100)", 100, labs(-100));
    TEST_ASSERT_EQUAL("labs(0)", 0, labs(0));
    return NULL;
}

/* Test div */
static const char *test_div(void) {
    div_t result;
    
    result = div(17, 5);
    TEST_ASSERT_EQUAL("div(17,5) quotient", 3, result.quot);
    TEST_ASSERT_EQUAL("div(17,5) remainder", 2, result.rem);
    
    result = div(-17, 5);
    TEST_ASSERT_EQUAL("div(-17,5) quotient", -3, result.quot);
    TEST_ASSERT_EQUAL("div(-17,5) remainder", -2, result.rem);
    
    result = div(20, 4);
    TEST_ASSERT_EQUAL("div(20,4) quotient", 5, result.quot);
    TEST_ASSERT_EQUAL("div(20,4) remainder", 0, result.rem);
    
    return NULL;
}

/* Test ldiv */
static const char *test_ldiv(void) {
    ldiv_t result;
    
    result = ldiv(100L, 7L);
    TEST_ASSERT_EQUAL("ldiv(100,7) quotient", 14, result.quot);
    TEST_ASSERT_EQUAL("ldiv(100,7) remainder", 2, result.rem);
    
    result = ldiv(50L, 10L);
    TEST_ASSERT_EQUAL("ldiv(50,10) quotient", 5, result.quot);
    TEST_ASSERT_EQUAL("ldiv(50,10) remainder", 0, result.rem);
    
    return NULL;
}

/* Test malloc/free */
static const char *test_malloc_free(void) {
    void *ptr;
    
    ptr = malloc(100);
    TEST_ASSERT_NOT_NULL("malloc(100) returns non-NULL", ptr);
    free(ptr);
    
    ptr = malloc(0);
    /* malloc(0) behavior is implementation-defined, just check it doesn't crash */
    free(ptr);
    
    ptr = malloc(1024);
    TEST_ASSERT_NOT_NULL("malloc(1024) returns non-NULL", ptr);
    free(ptr);
    
    return NULL;
}

/* Test calloc */
static const char *test_calloc(void) {
    int *ptr;
    int i;
    
    ptr = (int *)calloc(10, sizeof(int));
    TEST_ASSERT_NOT_NULL("calloc returns non-NULL", ptr);
    
    /* Check that memory is zeroed */
    for (i = 0; i < 10; i++) {
        if (ptr[i] != 0) {
            free(ptr);
            TEST_ASSERT("calloc zeros memory", 0);
        }
    }
    
    free(ptr);
    TEST_ASSERT("calloc zeros memory", 1);
    
    return NULL;
}

/* Test realloc */
static const char *test_realloc(void) {
    int *ptr;
    int *new_ptr;
    
    ptr = (int *)malloc(10 * sizeof(int));
    TEST_ASSERT_NOT_NULL("malloc for realloc test", ptr);
    
    ptr[0] = 42;
    ptr[9] = 99;
    
    new_ptr = (int *)realloc(ptr, 20 * sizeof(int));
    TEST_ASSERT_NOT_NULL("realloc returns non-NULL", new_ptr);
    TEST_ASSERT_EQUAL("realloc preserves data [0]", 42, new_ptr[0]);
    TEST_ASSERT_EQUAL("realloc preserves data [9]", 99, new_ptr[9]);
    
    free(new_ptr);
    
    return NULL;
}

/* Test rand */
static const char *test_rand(void) {
    int r1, r2;
    
    /* Set seed for deterministic behavior */
    srand(12345);
    r1 = rand();
    
    srand(12345);
    r2 = rand();
    
    TEST_ASSERT_EQUAL("rand with same seed produces same value", r1, r2);
    TEST_ASSERT("rand returns value in range", r1 >= 0 && r1 <= RAND_MAX);
    
    /* Generate a few more to ensure it works */
    r1 = rand();
    TEST_ASSERT("rand produces different value", r1 != r2);
    
    return NULL;
}

/* Test qsort */
static int compare_ints(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

static const char *test_qsort(void) {
    int arr[] = {5, 2, 8, 1, 9, 3};
    int expected[] = {1, 2, 3, 5, 8, 9};
    int i;
    
    qsort(arr, 6, sizeof(int), compare_ints);
    
    for (i = 0; i < 6; i++) {
        if (arr[i] != expected[i]) {
            TEST_ASSERT("qsort failed", 0);
        }
    }
    TEST_ASSERT("qsort sorted array", 1);
    
    return NULL;
}

/* Test bsearch */
static const char *test_bsearch(void) {
    int arr[] = {1, 2, 3, 5, 8, 9};
    int key = 5;
    int *result;
    
    result = (int *)bsearch(&key, arr, 6, sizeof(int), compare_ints);
    TEST_ASSERT_NOT_NULL("bsearch finds existing element", result);
    TEST_ASSERT_EQUAL("bsearch returns correct value", 5, *result);
    
    key = 7;
    result = (int *)bsearch(&key, arr, 6, sizeof(int), compare_ints);
    TEST_ASSERT_NULL("bsearch doesn't find missing element", result);
    
    return NULL;
}

/* Test getenv (basic test) */
static const char *test_getenv(void) {
    char *result;
    
    /* Set an environment variable for testing */
    putenv("TEST_VAR=test_value");
    
    result = getenv("TEST_VAR");
    TEST_ASSERT_NOT_NULL("getenv finds set variable", result);
    TEST_ASSERT_STR_EQUAL("getenv returns correct value", "test_value", result);
    
    result = getenv("NONEXISTENT_VAR_12345");
    TEST_ASSERT_NULL("getenv returns NULL for nonexistent var", result);
    
    return NULL;
}

/* Test system (if available) - basic test */
static const char *test_system(void) {
    int result;
    
    /* Test if system is available */
    result = system(NULL);
    if (result != 0) {
        /* system() is available, test a simple command */
#ifdef __AMIGA__
        result = system("endcli");
        TEST_ASSERT_EQUAL("system('endcli') returns 0", 0, result);
#else
        result = system("exit 0");
        TEST_ASSERT_EQUAL("system('exit 0') returns 0", 0, result);
#endif
    } else {
        /* system() not available, skip test */
        printf("    (system() not available, skipping)\n");
    }
    
    return NULL;
}

/* Main test runner */
int main(void) {
    BEGIN_TEST_SUITE("stdlib.h");
    
    RUN_TEST(test_atoi);
    RUN_TEST(test_atol);
    RUN_TEST(test_atof);
    RUN_TEST(test_strtol);
    RUN_TEST(test_strtoul);
    RUN_TEST(test_abs);
    RUN_TEST(test_labs);
    RUN_TEST(test_div);
    RUN_TEST(test_ldiv);
    RUN_TEST(test_malloc_free);
    RUN_TEST(test_calloc);
    RUN_TEST(test_realloc);
    RUN_TEST(test_rand);
    RUN_TEST(test_qsort);
    RUN_TEST(test_bsearch);
    RUN_TEST(test_getenv);
    RUN_TEST(test_system);
    
    END_TEST_SUITE();
    
    return tests_failed;
}
