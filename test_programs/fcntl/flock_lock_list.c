/*
 * flock() cost against the number of files locked elsewhere, plus the
 * lock semantics a faster lookup must keep.
 *
 * Every flock() and every close() of a locked descriptor used to walk the
 * system-wide lock list with a dos.library SameLock() per node, so one
 * lock-plus-close got slower with each file locked anywhere in the system.
 *
 * Run as:  flock_lock_list               (500 background locks, 200 rounds)
 *          flock_lock_list 1000 300      (background locks, rounds)
 *
 * The program runs copies of itself to act as a second process, so start
 * it with a path that can be run again (for example from its own drawer).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>

#define BASE "T:flock_lock_list"

static const char *self;
static int failures;

static void check(const char *what, int ok) {
    printf("  %s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok)
        failures++;
}

static long elapsed_us(const struct timeval *start) {
    struct timeval now;

    gettimeofday(&now, NULL);
    return (now.tv_sec - start->tv_sec) * 1000000L + (now.tv_usec - start->tv_usec);
}

static int exists(const char *path) {
    return access(path, F_OK) == 0;
}

static void touch(const char *path) {
    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    if (fd >= 0)
        close(fd);
}

static int wait_for(const char *path) {
    int i;

    for (i = 0; i < 500; i++) {
        if (exists(path))
            return 1;
        usleep(20000);
    }
    return 0;
}

/* Another process tries to take an exclusive lock: 0 = got it, 1 = blocked. */
static int child_try(const char *path) {
    char cmd[512];

    snprintf(cmd, sizeof(cmd), "%s try %s", self, path);
    return system(cmd);
}

/* Child: take LOCK_EX|LOCK_NB and say whether it worked. */
static int run_try(const char *path) {
    int fd = open(path, O_RDWR);
    int rc;

    if (fd < 0)
        return 2;
    if (flock(fd, LOCK_EX | LOCK_NB) == 0)
        rc = 0;
    else
        rc = (errno == EWOULDBLOCK || errno == EAGAIN || errno == EACCES) ? 1 : 2;
    close(fd);
    return rc;
}

/* Child: hold LOCK_SH until told to let go, then unlock and close. */
static int run_shhold(const char *path, const char *dir) {
    char name[256];
    int fd = open(path, O_RDWR);

    if (fd < 0 || flock(fd, LOCK_SH | LOCK_NB) != 0)
        return 2;
    snprintf(name, sizeof(name), "%s/ready", dir);
    touch(name);
    snprintf(name, sizeof(name), "%s/go", dir);
    wait_for(name);
    flock(fd, LOCK_UN);
    close(fd);
    snprintf(name, sizeof(name), "%s/done", dir);
    touch(name);
    return 0;
}

/* Lock, close and delete a new file 'rounds' times; returns µs per round. */
static long time_rounds(const char *dir, const char *fmt, int rounds) {
    struct timeval start;
    char name[256];
    int i;

    gettimeofday(&start, NULL);
    for (i = 0; i < rounds; i++) {
        int fd;

        snprintf(name, sizeof(name), fmt, dir, i);
        fd = open(name, O_CREAT | O_RDWR | O_TRUNC, 0666);
        if (fd < 0 || flock(fd, LOCK_EX | LOCK_NB) != 0) {
            printf("  round %d: open/flock of %s failed, errno %d\n", i, name, errno);
            failures++;
            if (fd >= 0)
                close(fd);
            return -1;
        }
        close(fd);
        unlink(name);
    }
    return elapsed_us(&start) / rounds;
}

/* Hold 'count' locked files; same_name puts them all at d<i>/same. */
static int *hold_locks(int count, int same_name) {
    int *fds = calloc(count > 0 ? count : 1, sizeof(int));
    char name[256];
    int i;

    for (i = 0; i < count; i++) {
        if (same_name) {
            snprintf(name, sizeof(name), BASE "/d%d", i);
            mkdir(name, 0777);
            snprintf(name, sizeof(name), BASE "/d%d/same", i);
        } else {
            snprintf(name, sizeof(name), BASE "/bg%d", i);
        }
        fds[i] = open(name, O_CREAT | O_RDWR, 0666);
        if (fds[i] < 0 || flock(fds[i], LOCK_EX | LOCK_NB) != 0) {
            printf("  could not lock background file %s, errno %d\n", name, errno);
            exit(20);
        }
    }
    return fds;
}

static void release_locks(int *fds, int count, int same_name) {
    char name[256];
    int i;

    for (i = 0; i < count; i++) {
        close(fds[i]);
        if (same_name) {
            snprintf(name, sizeof(name), BASE "/d%d/same", i);
            unlink(name);
            snprintf(name, sizeof(name), BASE "/d%d", i);
            rmdir(name);
        } else {
            snprintf(name, sizeof(name), BASE "/bg%d", i);
            unlink(name);
        }
    }
    free(fds);
}

static void timing(int count, int rounds) {
    long base, distinct, same;
    int *fds;

    printf("Timing: open + flock(LOCK_EX|LOCK_NB) + close + unlink, %d rounds\n", rounds);

    base = time_rounds(BASE, "%s/w%d", rounds);
    printf("  no other locks:                      %6ld us per round\n", base);

    fds = hold_locks(count, 0);
    distinct = time_rounds(BASE, "%s/w%d", rounds);
    printf("  %4d other locks, different names:   %6ld us per round\n", count, distinct);
    release_locks(fds, count, 0);

    /* Every held file is called "same", so a name compare cannot skip them. */
    fds = hold_locks(count, 1);
    same = time_rounds(BASE, "%s/same", rounds);
    printf("  %4d other locks, all named \"same\":  %6ld us per round\n", count, same);
    release_locks(fds, count, 1);
}

static void semantics(void) {
    char file[] = BASE "/sem";
    char dir[] = BASE "/sh";
    char name[256];
    int fd, fd2, dup_fd, i;

    printf("Lock semantics\n");
    touch(file);

    /* 1. An exclusive lock blocks another process until LOCK_UN. */
    fd = open(file, O_RDWR);
    check("LOCK_EX taken", flock(fd, LOCK_EX | LOCK_NB) == 0);
    check("other process is blocked while LOCK_EX is held", child_try(file) == 1);
    check("LOCK_UN", flock(fd, LOCK_UN) == 0);
    check("other process gets the lock after LOCK_UN", child_try(file) == 0);
    close(fd);

    /* 2. Two descriptors on one file in one process; close both. */
    fd = open(file, O_RDWR);
    fd2 = open(file, O_RDWR);
    check("first descriptor locked", flock(fd, LOCK_EX | LOCK_NB) == 0);
    check("second descriptor locked (same owner)", flock(fd2, LOCK_EX | LOCK_NB) == 0);
    close(fd);
    close(fd2);
    check("other process gets the lock after both closes", child_try(file) == 0);

    /* 3. A dup()ed descriptor keeps the lock after the original closes. */
    fd = open(file, O_RDWR);
    check("LOCK_EX taken before dup", flock(fd, LOCK_EX | LOCK_NB) == 0);
    dup_fd = dup(fd);
    close(fd);
    check("other process still blocked after closing the original", child_try(file) == 1);
    close(dup_fd);
    check("other process gets the lock after closing the dup", child_try(file) == 0);

    /* 4. We unlock a shared lock first, the other process frees the lock
     *    node, then we close. The close must not touch the freed node. */
    mkdir(dir, 0777);
    snprintf(name, sizeof(name), "%s/ready", dir); unlink(name);
    snprintf(name, sizeof(name), "%s/go", dir); unlink(name);
    snprintf(name, sizeof(name), "%s/done", dir); unlink(name);

    fd = open(file, O_RDWR);
    check("LOCK_SH taken", flock(fd, LOCK_SH | LOCK_NB) == 0);
    snprintf(name, sizeof(name), "Run >NIL: %s shhold %s %s", self, file, dir);
    system(name);
    snprintf(name, sizeof(name), "%s/ready", dir);
    check("other process took LOCK_SH too", wait_for(name));
    check("our LOCK_UN while the other still holds LOCK_SH", flock(fd, LOCK_UN) == 0);
    snprintf(name, sizeof(name), "%s/go", dir);
    touch(name);
    snprintf(name, sizeof(name), "%s/done", dir);
    check("other process unlocked and closed", wait_for(name));

    /* Reuse the freed memory with fresh lock nodes, then close. */
    for (i = 0; i < 20; i++) {
        snprintf(name, sizeof(name), BASE "/churn%d", i);
        fd2 = open(name, O_CREAT | O_RDWR, 0666);
        flock(fd2, LOCK_EX | LOCK_NB);
        close(fd2);
        unlink(name);
    }
    close(fd);
    printf("  still running after close()\n");
    check("other process gets the lock at the end", child_try(file) == 0);

    snprintf(name, sizeof(name), "%s/ready", dir); unlink(name);
    snprintf(name, sizeof(name), "%s/go", dir); unlink(name);
    snprintf(name, sizeof(name), "%s/done", dir); unlink(name);
    rmdir(dir);
    unlink(file);
}

int main(int argc, char *argv[]) {
    int count = 500, rounds = 200;

    self = argv[0];

    if (argc == 3 && strcmp(argv[1], "try") == 0)
        return run_try(argv[2]);
    if (argc == 4 && strcmp(argv[1], "shhold") == 0)
        return run_shhold(argv[2], argv[3]);

    if (argc > 1)
        count = atoi(argv[1]);
    if (argc > 2)
        rounds = atoi(argv[2]);
    if (rounds < 1)
        rounds = 1;

    mkdir(BASE, 0777);
    semantics();
    timing(count, rounds);
    rmdir(BASE);

    printf("%s\n", failures == 0 ? "ALL PASSED" : "FAILURES");
    return failures == 0 ? 0 : 10;
}
