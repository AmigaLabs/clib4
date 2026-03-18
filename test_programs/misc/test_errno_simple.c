/*
 * Simple errno test
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    printf("=== SIMPLE ERRNO TEST ===\n\n");

    // Test 1: Direct assignment
    printf("Test 1: Direct assignment\n");
    errno = 0;
    printf("  Set errno = 0, read: %d\n", errno);
    errno = 22;  // EINVAL
    printf("  Set errno = 22, read: %d\n", errno);
    printf("  strerror: %s\n\n", strerror(errno));

    // Test 2: Multiple direct assignments
    printf("Test 2: Multiple consecutive assignments\n");
    errno = 2;  // ENOENT
    printf("  errno = 2: read %d (%s)\n", errno, strerror(errno));
    errno = 13; // EACCES
    printf("  errno = 13: read %d (%s)\n", errno, strerror(errno));
    errno = 28; // ENOSPC
    printf("  errno = 28: read %d (%s)\n\n", errno, strerror(errno));

    // Test 3: Reset to zero
    printf("Test 3: Reset to zero\n");
    errno = 0;
    printf("  errno = 0: read %d\n", errno);

    if (errno == 0) {
        printf("\n+ SUCCESS: Direct errno manipulation works!\n");
        return 0;
    } else {
        printf("\nx FAILURE: errno is broken (expected 0, got %d)!\n", errno);
        return 1;
    }
}
