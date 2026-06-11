/*
 * pipe_posix_test.c — comprehensive POSIX pipe behavior test for clib4
 *
 * Covers:
 *  1.  Basic read/write
 *  2.  EOF: write-end closed before read
 *  3.  Non-blocking (pipe2 O_NONBLOCK): EAGAIN when no data
 *  4.  Non-blocking: data available → reads correctly
 *  5.  Non-blocking: write-end closed → read returns 0 (EOF)
 *  6.  Large data transfer (64 KB)
 *  7.  Multiple writes, single large read
 *  8.  EPIPE: write to pipe whose read-end is closed
 *  9.  Blocking read waits for data (pthread writer)
 * 10.  pipe2 O_CLOEXEC flag is set (FD_CLOEXEC)
 * 11.  read() on empty non-blocking pipe after partial drain → EAGAIN
 * 12.  write/read across thread boundary (producer/consumer)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>

/* ------------------------------------------------------------------ helpers */

static int passed = 0;
static int failed = 0;

#define PASS(name) do { printf("  [PASS] %s\n", name); passed++; } while(0)
#define FAIL(name, ...) do { printf("  [FAIL] %s: ", name); printf(__VA_ARGS__); printf("\n"); failed++; } while(0)

static long ms_now(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/* ------------------------------------------------------------------ test 1 */
static void test_basic_rw(void) {
    const char *name = "basic read/write";
    int fd[2];
    if (pipe(fd) != 0) { FAIL(name, "pipe() failed: %s", strerror(errno)); return; }

    const char *msg = "Hello, pipe!";
    ssize_t w = write(fd[1], msg, strlen(msg));
    if (w != (ssize_t)strlen(msg)) { FAIL(name, "write returned %ld", (long)w); goto done; }

    char buf[64] = {0};
    ssize_t r = read(fd[0], buf, sizeof(buf) - 1);
    if (r != (ssize_t)strlen(msg)) { FAIL(name, "read returned %ld", (long)r); goto done; }
    if (strcmp(buf, msg) != 0) { FAIL(name, "data mismatch: '%s'", buf); goto done; }

    PASS(name);
done:
    close(fd[0]); close(fd[1]);
}

/* ------------------------------------------------------------------ test 2 */
static void test_eof_on_close(void) {
    const char *name = "EOF when write-end closed";
    int fd[2];
    if (pipe(fd) != 0) { FAIL(name, "pipe() failed: %s", strerror(errno)); return; }

    write(fd[1], "x", 1);
    close(fd[1]);   /* close write-end */

    char buf[16];
    ssize_t r1 = read(fd[0], buf, sizeof(buf));  /* read the 'x' */
    ssize_t r2 = read(fd[0], buf, sizeof(buf));  /* should be EOF == 0 */

    if (r1 != 1) { FAIL(name, "first read returned %ld, expected 1", (long)r1); goto done; }
    if (r2 != 0) { FAIL(name, "EOF read returned %ld, expected 0", (long)r2); goto done; }

    PASS(name);
done:
    close(fd[0]);
}

/* ------------------------------------------------------------------ test 3 */
static void test_nonblock_eagain(void) {
    const char *name = "non-blocking: EAGAIN on empty pipe";
    int fd[2];
    if (pipe2(fd, O_NONBLOCK) != 0) { FAIL(name, "pipe2() failed: %s", strerror(errno)); return; }

    char buf[16];
    ssize_t r = read(fd[0], buf, sizeof(buf));
    if (r != -1 || errno != EAGAIN) {
        FAIL(name, "expected -1/EAGAIN, got r=%ld errno=%d (%s)", (long)r, errno, strerror(errno));
        goto done;
    }
    PASS(name);
done:
    close(fd[0]); close(fd[1]);
}

/* ------------------------------------------------------------------ test 4 */
static void test_nonblock_data(void) {
    const char *name = "non-blocking: data available";
    int fd[2];
    if (pipe2(fd, O_NONBLOCK) != 0) { FAIL(name, "pipe2() failed: %s", strerror(errno)); return; }

    write(fd[1], "NB", 2);

    char buf[8] = {0};
    ssize_t r = read(fd[0], buf, sizeof(buf));
    if (r != 2 || memcmp(buf, "NB", 2) != 0) {
        FAIL(name, "read returned %ld, data='%.*s'", (long)r, (int)r, buf);
        goto done;
    }
    PASS(name);
done:
    close(fd[0]); close(fd[1]);
}

/* ------------------------------------------------------------------ test 5 */
static void test_nonblock_eof(void) {
    const char *name = "non-blocking: EOF when write-end closed";
    int fd[2];
    if (pipe2(fd, O_NONBLOCK) != 0) { FAIL(name, "pipe2() failed: %s", strerror(errno)); return; }

    close(fd[1]);   /* close write-end immediately */

    char buf[8];
    ssize_t r = read(fd[0], buf, sizeof(buf));
    if (r != 0) {
        FAIL(name, "expected 0 (EOF), got %ld errno=%d (%s)", (long)r, errno, strerror(errno));
        goto done;
    }
    PASS(name);
done:
    close(fd[0]);
}

/* ------------------------------------------------------------------ test 6 */
#define LARGE_SIZE (64 * 1024)
static void test_large_data(void) {
    const char *name = "large data transfer (64 KB)";
    int fd[2];
    if (pipe(fd) != 0) { FAIL(name, "pipe() failed: %s", strerror(errno)); return; }

    char *wbuf = malloc(LARGE_SIZE);
    char *rbuf = malloc(LARGE_SIZE);
    if (!wbuf || !rbuf) { FAIL(name, "malloc failed"); free(wbuf); free(rbuf); goto done; }

    for (int i = 0; i < LARGE_SIZE; i++) wbuf[i] = (char)(i & 0xFF);

    /* Write in chunks so we don't block forever on a pipe with limited buffer */
    ssize_t total_w = 0;
    while (total_w < LARGE_SIZE) {
        ssize_t w = write(fd[1], wbuf + total_w, LARGE_SIZE - total_w);
        if (w <= 0) { FAIL(name, "write failed at offset %ld: %s", (long)total_w, strerror(errno)); goto done_free; }
        total_w += w;
    }

    ssize_t total_r = 0;
    while (total_r < LARGE_SIZE) {
        ssize_t r = read(fd[0], rbuf + total_r, LARGE_SIZE - total_r);
        if (r <= 0) { FAIL(name, "read failed at offset %ld: %s", (long)total_r, strerror(errno)); goto done_free; }
        total_r += r;
    }

    if (memcmp(wbuf, rbuf, LARGE_SIZE) != 0) { FAIL(name, "data mismatch"); goto done_free; }

    PASS(name);
done_free:
    free(wbuf); free(rbuf);
done:
    close(fd[0]); close(fd[1]);
}

/* ------------------------------------------------------------------ test 7 */
static void test_multiple_writes(void) {
    const char *name = "multiple writes, single read";
    int fd[2];
    if (pipe(fd) != 0) { FAIL(name, "pipe() failed: %s", strerror(errno)); return; }

    write(fd[1], "AAA", 3);
    write(fd[1], "BBB", 3);
    write(fd[1], "CCC", 3);

    char buf[16] = {0};
    ssize_t r = read(fd[0], buf, 9);
    if (r != 9) { FAIL(name, "read returned %ld, expected 9", (long)r); goto done; }
    if (memcmp(buf, "AAABBBCCC", 9) != 0) { FAIL(name, "data mismatch: '%.9s'", buf); goto done; }

    PASS(name);
done:
    close(fd[0]); close(fd[1]);
}

/* ------------------------------------------------------------------ test 8 */
static int got_sigpipe = 0;
static void handle_sigpipe(int sig) { (void)sig; got_sigpipe = 1; }

static void test_epipe(void) {
    const char *name = "EPIPE on write to closed read-end";
    int fd[2];
    if (pipe(fd) != 0) { FAIL(name, "pipe() failed: %s", strerror(errno)); return; }

    got_sigpipe = 0;
    signal(SIGPIPE, handle_sigpipe);

    close(fd[0]);   /* close read-end */

    ssize_t w = write(fd[1], "x", 1);
    if (w != -1 || errno != EPIPE) {
        FAIL(name, "expected -1/EPIPE, got w=%ld errno=%d (%s)", (long)w, errno, strerror(errno));
        goto done;
    }
    PASS(name);
done:
    signal(SIGPIPE, SIG_DFL);
    close(fd[1]);
}

/* ------------------------------------------------------------------ test 9 */
struct thread_write_arg { int fd; const char *data; int delay_ms; };

static void *thread_writer(void *arg) {
    struct thread_write_arg *a = arg;
    /* simulate delay before writing */
    struct timeval tv = { .tv_sec = a->delay_ms / 1000, .tv_usec = (a->delay_ms % 1000) * 1000 };
    select(0, NULL, NULL, NULL, &tv);
    write(a->fd, a->data, strlen(a->data));
    close(a->fd);
    return NULL;
}

static void test_blocking_wait(void) {
    const char *name = "blocking read waits for data (pthread writer)";
    int fd[2];
    if (pipe(fd) != 0) { FAIL(name, "pipe() failed: %s", strerror(errno)); return; }

    struct thread_write_arg arg = { .fd = fd[1], .data = "WAIT_OK", .delay_ms = 100 };
    pthread_t thr;
    if (pthread_create(&thr, NULL, thread_writer, &arg) != 0) {
        FAIL(name, "pthread_create failed: %s", strerror(errno));
        close(fd[0]); close(fd[1]);
        return;
    }

    long t0 = ms_now();
    char buf[16] = {0};
    ssize_t r = read(fd[0], buf, sizeof(buf) - 1);  /* must block ~100ms */
    long elapsed = ms_now() - t0;

    pthread_join(thr, NULL);
    close(fd[0]);

    if (r <= 0) { FAIL(name, "read returned %ld: %s", (long)r, strerror(errno)); return; }
    if (strcmp(buf, "WAIT_OK") != 0) { FAIL(name, "data mismatch: '%s'", buf); return; }
    if (elapsed < 50) { FAIL(name, "did not actually block (elapsed %ld ms)", elapsed); return; }

    PASS(name);
    (void)elapsed;
}

/* ----------------------------------------------------------------- test 10 */
static void test_pipe2_cloexec(void) {
    const char *name = "pipe2 O_CLOEXEC sets FD_CLOEXEC";
    int fd[2];
    if (pipe2(fd, O_CLOEXEC) != 0) { FAIL(name, "pipe2() failed: %s", strerror(errno)); return; }

    int f0 = fcntl(fd[0], F_GETFD);
    int f1 = fcntl(fd[1], F_GETFD);

    if (f0 == -1 || !(f0 & FD_CLOEXEC)) { FAIL(name, "read-end missing FD_CLOEXEC (flags=%d)", f0); goto done; }
    if (f1 == -1 || !(f1 & FD_CLOEXEC)) { FAIL(name, "write-end missing FD_CLOEXEC (flags=%d)", f1); goto done; }

    PASS(name);
done:
    close(fd[0]); close(fd[1]);
}

/* ----------------------------------------------------------------- test 11 */
static void test_nonblock_partial_drain(void) {
    const char *name = "non-blocking: EAGAIN after data drained";
    int fd[2];
    if (pipe2(fd, O_NONBLOCK) != 0) { FAIL(name, "pipe2() failed: %s", strerror(errno)); return; }

    write(fd[1], "AB", 2);

    char buf[8];
    ssize_t r1 = read(fd[0], buf, 2);   /* drain the data */
    ssize_t r2 = read(fd[0], buf, 2);   /* now empty → EAGAIN */

    if (r1 != 2) { FAIL(name, "first read returned %ld, expected 2", (long)r1); goto done; }
    if (r2 != -1 || errno != EAGAIN) {
        FAIL(name, "expected -1/EAGAIN after drain, got r=%ld errno=%d (%s)", (long)r2, errno, strerror(errno));
        goto done;
    }
    PASS(name);
done:
    close(fd[0]); close(fd[1]);
}

/* ----------------------------------------------------------------- test 12 */
#define PROD_MSGS  20
#define PROD_MSGLEN 8

struct prod_cons_arg { int rfd; int wfd; };

static void *producer(void *arg) {
    struct prod_cons_arg *a = arg;
    for (int i = 0; i < PROD_MSGS; i++) {
        char msg[PROD_MSGLEN];
        memset(msg, 'A' + (i % 26), PROD_MSGLEN);
        write(a->wfd, msg, PROD_MSGLEN);
    }
    close(a->wfd);
    return NULL;
}

static void test_producer_consumer(void) {
    const char *name = "producer/consumer across threads";
    int fd[2];
    if (pipe(fd) != 0) { FAIL(name, "pipe() failed: %s", strerror(errno)); return; }

    struct prod_cons_arg arg = { .rfd = fd[0], .wfd = fd[1] };
    pthread_t thr;
    if (pthread_create(&thr, NULL, producer, &arg) != 0) {
        FAIL(name, "pthread_create failed: %s", strerror(errno));
        close(fd[0]); close(fd[1]);
        return;
    }

    ssize_t total = 0;
    char buf[PROD_MSGLEN * 2];
    while (1) {
        ssize_t r = read(fd[0], buf, sizeof(buf));
        if (r == 0) break;   /* EOF: producer closed write-end */
        if (r < 0) { FAIL(name, "read error: %s", strerror(errno)); pthread_join(thr, NULL); close(fd[0]); return; }
        total += r;
    }

    pthread_join(thr, NULL);
    close(fd[0]);

    if (total != PROD_MSGS * PROD_MSGLEN) {
        FAIL(name, "received %ld bytes, expected %d", (long)total, PROD_MSGS * PROD_MSGLEN);
        return;
    }
    PASS(name);
}

/* ------------------------------------------------------------------ main */
int main(void) {
    printf("=== POSIX Pipe tests ===\n\n");

    test_basic_rw();
    test_eof_on_close();
    test_nonblock_eagain();
    test_nonblock_data();
    test_nonblock_eof();
    test_large_data();
    test_multiple_writes();
    test_epipe();
    test_blocking_wait();
    test_pipe2_cloexec();
    test_nonblock_partial_drain();
    test_producer_consumer();

    printf("\n=== Results: %d passed, %d failed ===\n", passed, failed);
    return (failed > 0) ? 1 : 0;
}
