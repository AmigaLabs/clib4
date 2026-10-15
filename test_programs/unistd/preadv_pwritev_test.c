/*
 * Tests for preadv() and pwritev(): the data transferred at the given offset,
 * that the file position is left unchanged, and the error cases.
 *
 * The test is built with -DNDEBUG, so it uses explicit checks, not assert().
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

static int failures = 0;

static void check(int ok, const char *what) {
    printf("%s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok)
        failures++;
}

int main(void) {
    const char *name = "preadv_pwritev_test.tmp";
    char first[5];
    char second[4];
    char whole[17];
    struct iovec rv[2];
    struct iovec wv[2];
    int fd;
    ssize_t n;
    off_t size;

    fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
    check(fd >= 0, "open temporary file");
    if (fd < 0)
        return 1;

    n = write(fd, "0123456789ABCDEF", 16);
    check(n == 16, "write initial contents");

    /* preadv reads at offset 4 and leaves the file position at 1. */
    lseek(fd, 1, SEEK_SET);
    rv[0].iov_base = first;
    rv[0].iov_len = sizeof(first);
    rv[1].iov_base = second;
    rv[1].iov_len = sizeof(second);
    n = preadv(fd, rv, 2, 4);
    check(n == 9, "preadv returns the total number of bytes");
    check(memcmp(first, "45678", 5) == 0, "preadv first segment is at offset 4");
    check(memcmp(second, "9ABC", 4) == 0, "preadv second segment follows it");
    check(lseek(fd, 0, SEEK_CUR) == 1, "preadv leaves the file position unchanged");

    n = preadv(fd, rv, 2, 100);
    check(n == 0, "preadv past end of file returns 0");

    /* pwritev writes at offset 10 and leaves the file position unchanged. */
    wv[0].iov_base = (void *)"XYZ";
    wv[0].iov_len = 3;
    wv[1].iov_base = (void *)"uvw";
    wv[1].iov_len = 3;
    n = pwritev(fd, wv, 2, 10);
    check(n == 6, "pwritev returns the total number of bytes");
    check(lseek(fd, 0, SEEK_CUR) == 1, "pwritev leaves the file position unchanged");

    lseek(fd, 0, SEEK_SET);
    n = read(fd, whole, 16);
    whole[16] = '\0';
    check(n == 16 && strcmp(whole, "0123456789XYZuvw") == 0, "pwritev contents are at offset 10");

    /* Writing past the end extends the file with zeros in the gap. */
    wv[0].iov_base = (void *)"Q";
    wv[0].iov_len = 1;
    n = pwritev(fd, wv, 1, 20);
    check(n == 1, "pwritev past end of file returns the byte count");
    size = lseek(fd, 0, SEEK_END);
    printf("     file size after pwritev at 20: %ld (errno %d)\n", (long)size, errno);
    check(size == 21, "pwritev past end extends the file to 21 bytes");

    /* Error cases. */
    n = preadv(-1, rv, 2, 0);
    check(n == -1 && errno == EBADF, "preadv on a bad descriptor fails with EBADF");

    n = pwritev(-1, wv, 1, 0);
    check(n == -1 && errno == EBADF, "pwritev on a bad descriptor fails with EBADF");

    errno = 0;
    n = preadv(fd, rv, 2, -1);
    printf("     preadv at offset -1 returned %ld (errno %d)\n", (long)n, errno);
    check(n == -1 && errno == EINVAL, "preadv with a negative offset fails with EINVAL");

    close(fd);
    unlink(name);

    printf("%s: %d failure(s)\n", failures ? "RESULT FAIL" : "RESULT OK", failures);
    return failures ? 1 : 0;
}
