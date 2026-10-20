/*
 * Tests for dup3(): descriptor duplication, the O_CLOEXEC flag, and the error
 * cases that differ from dup2().
 *
 * The test is built with -DNDEBUG, so it uses explicit checks, not assert().
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int failures = 0;

static void check(int ok, const char *what) {
    printf("%s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok)
        failures++;
}

int main(void) {
    const char *name = "dup3_test.tmp";
    char buf[8];
    int fd;
    int target;
    int result;
    ssize_t n;

    fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0644);
    check(fd >= 0, "open temporary file");
    if (fd < 0)
        return 1;

    n = write(fd, "hello", 5);
    check(n == 5, "write initial contents");

    /* Pick a free descriptor number to duplicate onto. */
    target = dup(fd);
    check(target >= 0, "dup to find a free descriptor");
    close(target);

    /* oldfd == newfd is EINVAL, unlike dup2(). */
    result = dup3(fd, fd, 0);
    check(result == -1 && errno == EINVAL, "dup3 with equal descriptors fails with EINVAL");

    /* Plain duplication onto a chosen descriptor, without close-on-exec. */
    result = dup3(fd, target, 0);
    check(result == target, "dup3 returns the requested descriptor");
    check((fcntl(target, F_GETFD) & FD_CLOEXEC) == 0, "dup3 without O_CLOEXEC leaves close-on-exec clear");

    /* The duplicate shares the file offset with the original. */
    lseek(fd, 0, SEEK_SET);
    n = read(target, buf, 5);
    buf[5] = '\0';
    check(n == 5 && strcmp(buf, "hello") == 0, "duplicate reads the same data");
    check(lseek(fd, 0, SEEK_CUR) == 5, "duplicate shares the file offset with the original");

    /* O_CLOEXEC sets close-on-exec on the new descriptor. */
    close(target);
    result = dup3(fd, target, O_CLOEXEC);
    check(result == target, "dup3 with O_CLOEXEC returns the requested descriptor");
    check((fcntl(target, F_GETFD) & FD_CLOEXEC) != 0, "dup3 with O_CLOEXEC sets close-on-exec");
    close(target);

    /* Flags other than O_CLOEXEC are rejected. */
    target = dup(fd);
    close(target);
    result = dup3(fd, target, 0x40000000);
    check(result == -1 && errno == EINVAL, "dup3 rejects unknown flags with EINVAL");

    /* A bad source descriptor fails. */
    result = dup3(-1, target, 0);
    check(result == -1 && errno == EBADF, "dup3 on a bad source descriptor fails with EBADF");

    close(fd);
    unlink(name);

    printf("%s: %d failure(s)\n", failures ? "RESULT FAIL" : "RESULT OK", failures);
    return failures ? 1 : 0;
}
