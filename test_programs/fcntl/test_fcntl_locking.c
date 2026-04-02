/*
 * Test program for fcntl() F_SETLK / F_GETLK / F_SETLKW
 * Focused on SQLite WAL compatibility testing.
 *
 * Build: ppc-amigaos-gcc -mcrt=clib4 -o test_fcntl_locking test_fcntl_locking.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define TEST_FILE "T:test_fcntl_lock.tmp"

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define CHECK(msg, cond) do { \
    tests_run++; \
    if (cond) { \
        tests_passed++; \
        printf("  + PASS: %s\n", msg); \
    } else { \
        tests_failed++; \
        printf("  x FAIL: %s\n", msg); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
    } \
} while(0)

#define CHECK_EQ(msg, expected, actual) do { \
    tests_run++; \
    long _e = (long)(expected), _a = (long)(actual); \
    if (_e == _a) { \
        tests_passed++; \
        printf("  + PASS: %s\n", msg); \
    } else { \
        tests_failed++; \
        printf("  x FAIL: %s (expected %ld, got %ld)\n", msg, _e, _a); \
        printf("    at %s:%d\n", __FILE__, __LINE__); \
    } \
} while(0)

/* Create a test file with some content */
static int create_test_file(void) {
    int fd = open(TEST_FILE, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) return -1;
    /* Write 4KB of data */
    char buf[4096];
    memset(buf, 'A', sizeof(buf));
    write(fd, buf, sizeof(buf));
    close(fd);
    return 0;
}

/* Test 1: Basic F_SETLK exclusive lock and F_UNLCK */
static void test_setlk_basic(void) {
    printf("\n--- Test 1: F_SETLK basic exclusive lock ---\n");

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 512;

    errno = 0;
    int ret = fcntl(fd, F_SETLK, &fl);
    int saved_errno = errno;
    CHECK_EQ("F_SETLK F_WRLCK should return 0", 0, ret);
    if (ret != 0) {
        printf("    errno = %d\n", saved_errno);
    }

    /* Unlock */
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 512;
    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("F_SETLK F_UNLCK should return 0", 0, ret);

    close(fd);
}

/* Test 2: Basic F_SETLK shared (read) lock */
static void test_setlk_shared(void) {
    printf("\n--- Test 2: F_SETLK shared lock ---\n");

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 512;

    errno = 0;
    int ret = fcntl(fd, F_SETLK, &fl);
    int saved_errno = errno;
    CHECK_EQ("F_SETLK F_RDLCK should return 0", 0, ret);
    if (ret != 0) {
        printf("    errno = %d\n", saved_errno);
    }

    /* Unlock */
    fl.l_type = F_UNLCK;
    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("F_SETLK F_UNLCK should return 0", 0, ret);

    close(fd);
}

/* Test 3: F_GETLK on unlocked file - THIS IS THE CRITICAL TEST
 * SQLite WAL calls F_GETLK to probe whether a lock would conflict.
 * POSIX: if no lock exists, l_type should be set to F_UNLCK. */
static void test_getlk_unlocked(void) {
    printf("\n--- Test 3: F_GETLK on unlocked file (critical for SQLite) ---\n");

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;     /* Ask: would an exclusive lock conflict? */
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 512;
    fl.l_pid = 0;

    printf("  Before F_GETLK: l_type=%d, l_start=%ld, l_len=%ld, l_pid=%d\n",
           fl.l_type, (long)fl.l_start, (long)fl.l_len, fl.l_pid);

    errno = 0;
    int ret = fcntl(fd, F_GETLK, &fl);
    int saved_errno = errno;

    printf("  After  F_GETLK: ret=%d, errno=%d, l_type=%d, l_start=%ld, l_len=%ld, l_pid=%d\n",
           ret, saved_errno, fl.l_type, (long)fl.l_start, (long)fl.l_len, fl.l_pid);

    CHECK_EQ("F_GETLK should return 0", 0, ret);
    CHECK_EQ("l_type should be F_UNLCK (no conflicting lock)", F_UNLCK, fl.l_type);

    close(fd);
}

/* Test 4: F_GETLK after F_SETLK - check lock query from same process */
static void test_getlk_after_setlk(void) {
    printf("\n--- Test 4: F_GETLK after F_SETLK (same process) ---\n");

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    /* Set an exclusive lock on bytes 0-511 */
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 512;

    int ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("F_SETLK should succeed", 0, ret);

    /* Now query: would a write lock on 0-511 conflict?
     * POSIX: same process locks never conflict with themselves in F_GETLK,
     * so l_type should be F_UNLCK.  */
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 512;

    errno = 0;
    ret = fcntl(fd, F_GETLK, &fl);
    int saved_errno = errno;

    printf("  F_GETLK after own lock: ret=%d, errno=%d, l_type=%d\n",
           ret, saved_errno, fl.l_type);

    CHECK_EQ("F_GETLK should return 0", 0, ret);
    /* Same process' own lock should not conflict */
    CHECK_EQ("l_type should be F_UNLCK (own locks don't conflict)", F_UNLCK, fl.l_type);

    /* Unlock */
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 512;
    fcntl(fd, F_SETLK, &fl);

    close(fd);
}

/* Test 5: SQLite WAL lock pattern
 * SQLite uses specific byte ranges in the WAL-index file:
 *   SHARED_FIRST  = 120 (byte offset)
 *   SHARED_SIZE   = 6   (number of bytes)
 *   EXCLUSIVE lock on byte 120 = WAL write lock
 *   SHARED lock on bytes 120-125 = WAL read lock
 *   F_GETLK probe before acquiring locks
 */
static void test_sqlite_wal_pattern(void) {
    printf("\n--- Test 5: SQLite WAL lock pattern ---\n");

    #define WAL_SHARED_FIRST 120
    #define WAL_SHARED_SIZE  6

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    /* Step 1: Probe with F_GETLK (SQLite does this first) */
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = WAL_SHARED_FIRST;
    fl.l_len = WAL_SHARED_SIZE;

    errno = 0;
    int ret = fcntl(fd, F_GETLK, &fl);
    int saved_errno = errno;
    printf("  WAL probe: ret=%d, errno=%d, l_type=%d\n", ret, saved_errno, fl.l_type);
    CHECK_EQ("WAL probe F_GETLK should return 0", 0, ret);

    /* Step 2: Take shared lock (WAL read lock) */
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = WAL_SHARED_FIRST;
    fl.l_len = 1;    /* SQLite locks one byte for shared */

    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("WAL shared lock F_SETLK should return 0", 0, ret);

    /* Step 3: Take exclusive lock (WAL write lock on different range) */
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = WAL_SHARED_FIRST + 1;
    fl.l_len = 1;

    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("WAL exclusive lock F_SETLK should return 0", 0, ret);

    /* Step 4: Unlock shared */
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = WAL_SHARED_FIRST;
    fl.l_len = 1;
    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("WAL unlock shared should return 0", 0, ret);

    /* Step 5: Unlock exclusive */
    fl.l_type = F_UNLCK;
    fl.l_start = WAL_SHARED_FIRST + 1;
    fl.l_len = 1;
    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("WAL unlock exclusive should return 0", 0, ret);

    close(fd);
}

/* Test 6: Lock with l_len=0 (lock to end of file)
 * SQLite uses this for the PENDING byte lock and RESERVED byte lock */
static void test_lock_to_eof(void) {
    printf("\n--- Test 6: Lock to EOF (l_len=0) ---\n");

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 1000;
    fl.l_len = 0;    /* Lock from offset 1000 to EOF */

    errno = 0;
    int ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("F_SETLK to EOF should return 0", 0, ret);

    /* Unlock */
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 1000;
    fl.l_len = 0;
    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("F_UNLCK to EOF should return 0", 0, ret);

    close(fd);
}

/* Test 7: F_GETLK with l_len=0 */
static void test_getlk_eof(void) {
    printf("\n--- Test 7: F_GETLK with l_len=0 (to EOF) ---\n");

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;    /* Would a lock from 0 to EOF conflict? */

    errno = 0;
    int ret = fcntl(fd, F_GETLK, &fl);
    int saved_errno = errno;
    printf("  F_GETLK (0..EOF): ret=%d, errno=%d, l_type=%d\n",
           ret, saved_errno, fl.l_type);

    CHECK_EQ("F_GETLK to EOF should return 0", 0, ret);
    CHECK_EQ("l_type should be F_UNLCK (nothing locked)", F_UNLCK, fl.l_type);

    close(fd);
}

/* Test 8: Multiple non-overlapping locks (SQLite uses several byte ranges) */
static void test_multiple_locks(void) {
    printf("\n--- Test 8: Multiple non-overlapping locks ---\n");

    int fd = open(TEST_FILE, O_RDWR);
    CHECK("open should succeed", fd >= 0);
    if (fd < 0) return;

    struct flock fl;

    /* Lock region 1: bytes 0-99 shared */
    memset(&fl, 0, sizeof(fl));
    fl.l_type = F_RDLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 100;
    int ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("Lock region 0-99 shared should succeed", 0, ret);

    /* Lock region 2: bytes 200-299 exclusive */
    fl.l_type = F_WRLCK;
    fl.l_start = 200;
    fl.l_len = 100;
    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("Lock region 200-299 exclusive should succeed", 0, ret);

    /* Lock region 3: bytes 500-599 shared */
    fl.l_type = F_RDLCK;
    fl.l_start = 500;
    fl.l_len = 100;
    ret = fcntl(fd, F_SETLK, &fl);
    CHECK_EQ("Lock region 500-599 shared should succeed", 0, ret);

    /* Unlock all */
    fl.l_type = F_UNLCK;
    fl.l_start = 0;
    fl.l_len = 100;
    fcntl(fd, F_SETLK, &fl);

    fl.l_start = 200;
    fcntl(fd, F_SETLK, &fl);

    fl.l_start = 500;
    fcntl(fd, F_SETLK, &fl);

    close(fd);
}

/* Test 9: Verify constant values used by SQLite */
static void test_constants(void) {
    printf("\n--- Test 9: Verify fcntl constants ---\n");

    printf("  F_RDLCK  = %d\n", F_RDLCK);
    printf("  F_UNLCK  = %d\n", F_UNLCK);
    printf("  F_WRLCK  = %d\n", F_WRLCK);
    printf("  F_GETLK  = %d\n", F_GETLK);
    printf("  F_SETLK  = %d\n", F_SETLK);
    printf("  F_SETLKW = %d\n", F_SETLKW);
    printf("  sizeof(struct flock) = %lu\n", (unsigned long)sizeof(struct flock));
    printf("  sizeof(off_t)        = %lu\n", (unsigned long)sizeof(off_t));
    printf("  sizeof(pid_t)        = %lu\n", (unsigned long)sizeof(pid_t));

    /* POSIX standard values for reference:
     * F_RDLCK=0, F_WRLCK=1, F_UNLCK=2 (Linux)
     * or F_RDLCK=1, F_UNLCK=2, F_WRLCK=3 (clib4)
     *
     * F_GETLK=5, F_SETLK=6, F_SETLKW=7 (Linux)
     * or F_GETLK=100, F_SETLK=101, F_SETLKW=102 (clib4)
     *
     * SQLite defines its own if needed, but typically uses system values.
     */
    CHECK("F_RDLCK should be defined", F_RDLCK >= 0);
    CHECK("F_WRLCK should be defined", F_WRLCK >= 0);
    CHECK("F_UNLCK should be defined", F_UNLCK >= 0);
    CHECK("F_RDLCK != F_WRLCK", F_RDLCK != F_WRLCK);
    CHECK("F_RDLCK != F_UNLCK", F_RDLCK != F_UNLCK);
    CHECK("F_WRLCK != F_UNLCK", F_WRLCK != F_UNLCK);
}

int main(void) {
    printf("==========================================================\n");
    printf(" fcntl() record locking test (SQLite WAL compatibility)\n");
    printf("==========================================================\n");

    if (create_test_file() < 0) {
        printf("FATAL: Cannot create test file %s\n", TEST_FILE);
        return 1;
    }

    test_setlk_basic();
    test_setlk_shared();
    test_getlk_unlocked();
    test_getlk_after_setlk();
    test_sqlite_wal_pattern();
    test_lock_to_eof();
    test_getlk_eof();
    test_multiple_locks();
    test_constants();

    /* Cleanup */
    unlink(TEST_FILE);

    printf("\n==========================================================\n");
    printf(" Summary: %d/%d passed, %d failed\n", tests_passed, tests_run, tests_failed);
    printf("==========================================================\n");

    return tests_failed > 0 ? 1 : 0;
}
