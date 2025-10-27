#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>     // open() flags
#include <unistd.h>    // close(), unlink()
#include <errno.h>     // errno
#include <string.h>    // strerror()
#include <sys/stat.h>  // mode_t

int i=1;

// Helper: describe open() flags in a human-readable way
void print_flags(int flags)
{
    printf("  Flags: 0x%x (", flags);
    if ((flags & O_ACCMODE) == O_RDONLY) printf("O_RDONLY ");
    if ((flags & O_ACCMODE) == O_WRONLY) printf("O_WRONLY ");
    if ((flags & O_ACCMODE) == O_RDWR)   printf("O_RDWR ");
    if (flags & O_CREAT)    printf("| O_CREAT ");
    if (flags & O_EXCL)     printf("| O_EXCL ");
    if (flags & O_TRUNC)    printf("| O_TRUNC ");
    if (flags & O_APPEND)   printf("| O_APPEND ");
    if (flags & O_SYNC)     printf("| O_SYNC ");
    if (flags & O_NONBLOCK) printf("| O_NONBLOCK ");
    if (flags & O_CLOEXEC)  printf("| O_CLOEXEC ");
    printf(")\n");
}

// Helper: safely open a file and report result
void test_open(const char *filename, int flags, mode_t mode)
{
    printf("\n=== Testing open(\"%s\") ===\n", filename);
    print_flags(flags);

    int fd = open(filename, flags, mode);

    if (fd == -1) {
        printf("%ld] open() failed: %s (errno=%d)\n", i, strerror(errno), errno);
    } else {
        printf("%ld] open() succeeded (fd=%d)\n", i, fd);
        close(fd);
    }
    i++;
}

int main(void)
{
    printf("Starting open() tests...\n");

    // Create a base file for existing-file tests
    unlink("existing.txt");
    int fd = open("existing.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "sample", 6);
        close(fd);
    }

    // Valid combinations
    test_open("existing.txt", O_RDONLY, 0);
    test_open("existing.txt", O_WRONLY, 0);
    test_open("existing.txt", O_RDWR, 0);
    test_open("existing.txt", O_RDWR | O_APPEND, 0);
    test_open("newfile.txt", O_CREAT | O_WRONLY, 0644);
    test_open("newfile2.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    test_open("newfile3.txt", O_CREAT | O_EXCL | O_WRONLY, 0644);

    // Intentional failures
    test_open("nonexistent.txt", O_RDONLY, 0);                     // file doesn’t exist
    test_open("nonexistent.txt", O_RDWR, 0);                       // file doesn’t exist, no O_CREAT
    test_open("existing.txt", O_EXCL, 0);                          // missing O_CREAT
    test_open("existing.txt", O_TRUNC, 0);                         // truncation requires write
    test_open("existing.txt", O_WRONLY | O_EXCL, 0);               // missing O_CREAT
    test_open("existing.txt", O_WRONLY | O_CREAT | O_EXCL, 0644);  // file exists already

    // Advanced flags
#ifdef O_CLOEXEC
    test_open("existing.txt", O_RDONLY | O_CLOEXEC, 0);
#endif
#ifdef O_SYNC
    test_open("existing.txt", O_WRONLY | O_SYNC, 0);
#endif
#ifdef O_NONBLOCK
#ifndef __amigaos4__
    test_open("/dev/null", O_RDONLY | O_NONBLOCK, 0);
#else
    test_open("NIL:", O_RDONLY | O_NONBLOCK, 0);
#endif
#endif

    printf("\nAll tests completed.\n");

    // --- Cleanup ---
    printf("\nCleaning up test files...\n");
    const char *files[] = {
        "existing.txt",
        "newfile.txt",
        "newfile2.txt",
        "newfile3.txt",
        "nonexistent.txt", // in case it was created accidentally
        NULL
    };

    for (int i = 0; files[i]; ++i) {
        if (unlink(files[i]) == 0)
            printf("  Removed %s\n", files[i]);
    }

    printf("Cleanup complete.\n");
    return EXIT_SUCCESS;
}
