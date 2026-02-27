/*
 * Test strerror() with errno = 0 and other common error codes
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void) {
    printf("=== STRERROR TEST ===\n\n");

    /* Test errno = 0 (Success) */
    printf("Test 1: strerror(0)\n");
    const char *msg0 = strerror(0);
    printf("  Result: '%s'\n", msg0);
    if (strcmp(msg0, "Success") == 0) {
        printf("  + PASS: strerror(0) returns 'Success'\n");
    } else {
        printf("  x FAIL: Expected 'Success', got '%s'\n", msg0);
    }
    printf("\n");

    /* Test common error codes */
    struct {
        int errnum;
        const char *expected;
    } tests[] = {
        {EPERM,   "Operation not permitted"},
        {ENOENT,  "No such file or directory"},
        {EINTR,   "Interrupted system call"},
        {EIO,     "Input/output error"},
        {EBADF,   "Bad file descriptor"},
        {ENOMEM,  "Cannot allocate memory"},
        {EACCES,  "Permission denied"},
        {EFAULT,  "Bad address"},
        {EINVAL,  "Invalid argument"},
        {EPIPE,   "Broken pipe"},
        {0, NULL} /* Sentinel */
    };

    int passed = 1;  /* Start with errno=0 test result */
    int failed = 0;
    int total = 1;   /* Count errno=0 test */

    printf("Testing common error codes:\n");
    for (int i = 0; tests[i].expected != NULL; i++) {
        total++;
        const char *msg = strerror(tests[i].errnum);
        printf("  errno %2d: '%s'\n", tests[i].errnum, msg);

        if (strcmp(msg, tests[i].expected) == 0) {
            passed++;
        } else {
            printf("    x Expected: '%s'\n", tests[i].expected);
            failed++;
        }
    }

    printf("\n");

    /* Test invalid error code */
    printf("Test: strerror(-1) (invalid error code)\n");
    const char *msg_invalid = strerror(-1);
    printf("  Result: '%s'\n", msg_invalid);
    if (strstr(msg_invalid, "Unknown error") != NULL) {
        printf("  + PASS: Invalid error code handled correctly\n");
        passed++;
    } else {
        printf("  x FAIL: Should return 'Unknown error -1'\n");
        failed++;
    }
    total++;

    printf("\n");

    /* Test very large error code */
    printf("Test: strerror(99999) (out of range)\n");
    const char *msg_large = strerror(99999);
    printf("  Result: '%s'\n", msg_large);
    if (strstr(msg_large, "Unknown error") != NULL) {
        printf("  + PASS: Out of range error handled correctly\n");
        passed++;
    } else {
        printf("  x FAIL: Should return 'Unknown error 99999'\n");
        failed++;
    }
    total++;

    /* Summary */
    printf("\n=== SUMMARY ===\n");
    printf("Total tests: %d\n", total);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);

    if (failed == 0) {
        printf("\n+ ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("\nx SOME TESTS FAILED!\n");
        return 1;
    }
}
