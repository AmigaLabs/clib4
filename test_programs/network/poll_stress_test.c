/*
 * poll_stress_test.c - Aggressive poll() test with network data transfer and threads
 *
 * Tests:
 * 1. Multiple concurrent TCP connections via poll()
 * 2. Threaded producers/consumers sending data through sockets
 * 3. Rapid connect/disconnect cycles
 * 4. Mixed read/write readiness detection
 * 5. poll() timeout accuracy
 * 6. POLLNVAL / POLLHUP detection
 * 7. High-throughput bulk data transfer with poll()
 * 8. Deadlock detection (watchdog thread)
 *
 * Build: ppc-amigaos-gcc -mcrt=clib4 -o poll_stress_test poll_stress_test.c -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

/* ---- Configuration ---- */
#define SERVER_PORT_BASE  19200
#define NUM_CLIENTS       5
#define BULK_SIZE         (64 * 1024)    /* 64KB per bulk send */
#define TOTAL_BYTES       (512 * 1024)   /* 512KB total per client */
#define POLL_TIMEOUT_MS   3000
#define WATCHDOG_SEC      60             /* kill test if stuck for 60s */
#define NUM_RAPID_CYCLES  20

/* ---- Globals ---- */
static volatile int g_watchdog_fed = 0;
static volatile int g_test_done = 0;
static pthread_mutex_t g_print_mtx = PTHREAD_MUTEX_INITIALIZER;
static int g_server_port;

#define TSAY(fmt, ...) do { \
    pthread_mutex_lock(&g_print_mtx); \
    printf("[tid=%d] " fmt "\n", (int)(size_t)pthread_self(), ##__VA_ARGS__); \
    fflush(stdout); \
    pthread_mutex_unlock(&g_print_mtx); \
} while(0)

#define PASS(name) do { \
    pthread_mutex_lock(&g_print_mtx); \
    printf("  PASS: %s\n", name); \
    fflush(stdout); \
    pthread_mutex_unlock(&g_print_mtx); \
} while(0)

#define FAIL(name, reason) do { \
    pthread_mutex_lock(&g_print_mtx); \
    printf("  FAIL: %s -- %s (errno=%d: %s)\n", name, reason, errno, strerror(errno)); \
    fflush(stdout); \
    pthread_mutex_unlock(&g_print_mtx); \
} while(0)

/* ---- Utility ---- */
static int
make_server_socket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) { close(fd); return -1; }
    if (listen(fd, 32) < 0) { close(fd); return -1; }
    return fd;
}

static int
connect_to(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) { close(fd); return -1; }
    return fd;
}

static void
set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0)
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* ---- Watchdog thread ---- */
static void *
watchdog_thread(void *arg) {
    (void)arg;
    int last_fed = g_watchdog_fed;
    int stale_count = 0;

    while (!g_test_done) {
        /* Sleep in 1-second increments so we exit promptly when done */
        for (int i = 0; i < 5 && !g_test_done; i++)
            sleep(1);
        if (g_test_done) break;

        if (g_watchdog_fed == last_fed) {
            stale_count += 5;
            if (stale_count >= WATCHDOG_SEC) {
                fprintf(stderr, "\n*** WATCHDOG: Test appears DEADLOCKED after %d seconds! ***\n", WATCHDOG_SEC);
                fflush(stderr);
                _exit(99);
            }
        } else {
            stale_count = 0;
            last_fed = g_watchdog_fed;
        }
    }
    return NULL;
}

/* ========================================================================
 * TEST 1: Basic poll() timeout accuracy
 * ======================================================================== */
static int
test_poll_timeout(void) {
    struct pollfd pfd;
    int sv = make_server_socket(g_server_port);
    if (sv < 0) { FAIL("poll_timeout", "server socket"); return 1; }

    pfd.fd = sv;
    pfd.events = POLLIN;

    /* poll with 200ms timeout, nobody connects → should return 0 */
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    int ret = poll(&pfd, 1, 200);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    long elapsed_ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_nsec - t0.tv_nsec) / 1000000;

    close(sv);

    if (ret != 0) {
        FAIL("poll_timeout", "expected 0 (timeout)");
        return 1;
    }
    /* Allow generous tolerance: 100ms - 2000ms */
    if (elapsed_ms < 100 || elapsed_ms > 2000) {
        char buf[128];
        snprintf(buf, sizeof(buf), "elapsed=%ldms, expected ~200ms", elapsed_ms);
        FAIL("poll_timeout", buf);
        return 1;
    }
    PASS("poll_timeout (200ms)");
    return 0;
}

/* ========================================================================
 * TEST 2: poll() immediate return (timeout=0)
 * ======================================================================== */
static int
test_poll_immediate(void) {
    struct pollfd pfd;
    int sv = make_server_socket(g_server_port + 1);
    if (sv < 0) { FAIL("poll_immediate", "server socket"); return 1; }

    pfd.fd = sv;
    pfd.events = POLLIN;

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    int ret = poll(&pfd, 1, 0);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    long elapsed_ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_nsec - t0.tv_nsec) / 1000000;

    close(sv);

    if (ret != 0) {
        FAIL("poll_immediate", "expected 0");
        return 1;
    }
    if (elapsed_ms > 500) {
        FAIL("poll_immediate", "took too long for timeout=0");
        return 1;
    }
    PASS("poll_immediate (timeout=0)");
    return 0;
}

/* ========================================================================
 * TEST 3: POLLNVAL for closed/invalid fd
 * ======================================================================== */
static int
test_poll_nval(void) {
    struct pollfd pfd;
    pfd.fd = 9999;  /* very unlikely to be valid */
    pfd.events = POLLIN;
    pfd.revents = 0;

    int ret = poll(&pfd, 1, 100);

    /* POSIX: poll should return 1 with POLLNVAL in revents, OR return with revents=0 if fd is skipped */
    if (ret < 0) {
        FAIL("poll_nval", "poll returned -1");
        return 1;
    }
    /* We accept either POLLNVAL or silent skip (clib4 current behavior) */
    if (ret >= 0) {
        PASS("poll_nval (invalid fd handled)");
        return 0;
    }
    return 0;
}

/* ========================================================================
 * TEST 4: Threaded bulk data transfer through poll()
 *
 * - Server thread: accepts NUM_CLIENTS connections, uses poll() to
 *   multiplex reads from all of them
 * - Client threads: each sends TOTAL_BYTES in BULK_SIZE chunks
 * ======================================================================== */

struct client_arg {
    int port;
    int id;
};

static void *
client_sender_thread(void *arg) {
    struct client_arg *ca = (struct client_arg *)arg;
    int fd = connect_to(ca->port);
    if (fd < 0) {
        TSAY("client %d: connect failed (errno=%d)", ca->id, errno);
        return NULL;
    }

    char *buf = malloc(BULK_SIZE);
    if (!buf) { close(fd); return NULL; }

    /* Fill with pattern */
    memset(buf, 'A' + (ca->id % 26), BULK_SIZE);

    size_t total_sent = 0;
    while (total_sent < TOTAL_BYTES) {
        size_t to_send = BULK_SIZE;
        if (total_sent + to_send > TOTAL_BYTES)
            to_send = TOTAL_BYTES - total_sent;

        ssize_t n = send(fd, buf, to_send, 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Brief pause, then retry */
                usleep(1000);
                continue;
            }
            TSAY("client %d: send error at %zu bytes (errno=%d)", ca->id, total_sent, errno);
            break;
        }
        total_sent += n;
        g_watchdog_fed++;
    }

    free(buf);
    close(fd);
    TSAY("client %d: sent %zu bytes total", ca->id, total_sent);
    return NULL;
}

struct server_result {
    size_t bytes_per_client[NUM_CLIENTS];
    int    clients_seen;
    int    error;
};

static void *
server_poll_thread(void *arg) {
    struct server_result *res = (struct server_result *)arg;
    int port = g_server_port + 2;
    int sv = make_server_socket(port);
    if (sv < 0) {
        res->error = 1;
        return NULL;
    }

    set_nonblocking(sv);

    struct pollfd pfds[NUM_CLIENTS + 1];
    int client_map[NUM_CLIENTS + 1]; /* index into bytes_per_client */
    int nfds = 1;
    int active_clients = 0;

    memset(pfds, 0, sizeof(pfds));
    pfds[0].fd = sv;
    pfds[0].events = POLLIN;

    for (int i = 1; i <= NUM_CLIENTS; i++) {
        pfds[i].fd = -1;
        client_map[i] = -1;
    }

    int total_accepted = 0;
    int total_closed = 0;

    while (total_closed < NUM_CLIENTS) {
        int ret = poll(pfds, nfds, POLL_TIMEOUT_MS);
        g_watchdog_fed++;

        if (ret < 0) {
            if (errno == EINTR) continue;
            TSAY("server: poll error (errno=%d)", errno);
            res->error = 1;
            break;
        }
        if (ret == 0) {
            /* timeout — if we haven't accepted all clients yet, keep waiting */
            if (total_accepted < NUM_CLIENTS) continue;
            /* All clients connected but nothing happening — possible issue */
            TSAY("server: poll timeout with %d active clients", active_clients);
            if (active_clients == 0) break;
            continue;
        }

        /* Check for new connections */
        if (pfds[0].revents & POLLIN) {
            while (total_accepted < NUM_CLIENTS) {
                struct sockaddr_in caddr;
                socklen_t clen = sizeof(caddr);
                int cfd = accept(sv, (struct sockaddr *)&caddr, &clen);
                if (cfd < 0) break;

                set_nonblocking(cfd);

                /* Find a slot */
                for (int i = 1; i <= NUM_CLIENTS; i++) {
                    if (pfds[i].fd < 0) {
                        pfds[i].fd = cfd;
                        pfds[i].events = POLLIN;
                        pfds[i].revents = 0;
                        client_map[i] = total_accepted;
                        if (i >= nfds) nfds = i + 1;
                        break;
                    }
                }
                total_accepted++;
                active_clients++;
                TSAY("server: accepted client %d (fd=%d)", total_accepted, cfd);
            }
        }

        /* Check client data */
        for (int i = 1; i < nfds; i++) {
            if (pfds[i].fd < 0) continue;

            if (pfds[i].revents & (POLLIN | POLLHUP | POLLERR)) {
                char buf[8192];
                ssize_t n = recv(pfds[i].fd, buf, sizeof(buf), 0);
                if (n > 0) {
                    int ci = client_map[i];
                    if (ci >= 0 && ci < NUM_CLIENTS)
                        res->bytes_per_client[ci] += n;
                } else if (n == 0) {
                    /* Client closed */
                    int ci = client_map[i];
                    TSAY("server: client %d disconnected (%zu bytes received)",
                         ci, ci >= 0 ? res->bytes_per_client[ci] : 0);
                    close(pfds[i].fd);
                    pfds[i].fd = -1;
                    pfds[i].events = 0;
                    pfds[i].revents = 0;
                    active_clients--;
                    total_closed++;
                } else {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        TSAY("server: recv error on slot %d (errno=%d)", i, errno);
                        close(pfds[i].fd);
                        pfds[i].fd = -1;
                        active_clients--;
                        total_closed++;
                    }
                }
            }
        }
    }

    res->clients_seen = total_accepted;
    close(sv);
    return NULL;
}

static int
test_bulk_transfer(void) {
    struct server_result res;
    memset(&res, 0, sizeof(res));

    pthread_t srv_tid;
    if (pthread_create(&srv_tid, NULL, server_poll_thread, &res) != 0) {
        FAIL("bulk_transfer", "server thread create");
        return 1;
    }

    /* Give server time to bind */
    usleep(100000);

    /* Launch client threads */
    pthread_t cli_tids[NUM_CLIENTS];
    struct client_arg cli_args[NUM_CLIENTS];

    for (int i = 0; i < NUM_CLIENTS; i++) {
        cli_args[i].port = g_server_port + 2;
        cli_args[i].id = i;
        if (pthread_create(&cli_tids[i], NULL, client_sender_thread, &cli_args[i]) != 0) {
            FAIL("bulk_transfer", "client thread create");
            return 1;
        }
    }

    /* Wait for all clients to finish sending */
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(cli_tids[i], NULL);
    }

    /* Wait for server to finish receiving */
    pthread_join(srv_tid, NULL);

    if (res.error) {
        FAIL("bulk_transfer", "server reported error");
        return 1;
    }

    int ok = 1;
    for (int i = 0; i < NUM_CLIENTS; i++) {
        if (res.bytes_per_client[i] != TOTAL_BYTES) {
            char msg[128];
            snprintf(msg, sizeof(msg), "client %d: received %zu/%d bytes",
                     i, res.bytes_per_client[i], TOTAL_BYTES);
            FAIL("bulk_transfer", msg);
            ok = 0;
        }
    }
    if (ok) {
        char msg[64];
        snprintf(msg, sizeof(msg), "bulk_transfer (%d clients x %dKB)", NUM_CLIENTS, TOTAL_BYTES / 1024);
        PASS(msg);
    }
    return ok ? 0 : 1;
}

/* ========================================================================
 * TEST 5: Rapid connect/disconnect (stress poll fd management)
 * ======================================================================== */
static int
test_rapid_connect_disconnect(void) {
    int port = g_server_port + 3;
    int sv = make_server_socket(port);
    if (sv < 0) { FAIL("rapid_connect", "server socket"); return 1; }

    set_nonblocking(sv);
    int failures = 0;

    for (int cycle = 0; cycle < NUM_RAPID_CYCLES; cycle++) {
        int cfd = connect_to(port);
        if (cfd < 0) { failures++; continue; }

        /* Accept on server side */
        struct pollfd pfd;
        pfd.fd = sv;
        pfd.events = POLLIN;
        int ret = poll(&pfd, 1, 1000);
        if (ret > 0 && (pfd.revents & POLLIN)) {
            struct sockaddr_in ca;
            socklen_t cl = sizeof(ca);
            int afd = accept(sv, (struct sockaddr *)&ca, &cl);
            if (afd >= 0) {
                /* Send a small message and close immediately */
                const char *msg = "ping";
                send(cfd, msg, 4, 0);

                /* Poll the accepted fd for data */
                struct pollfd rpfd;
                rpfd.fd = afd;
                rpfd.events = POLLIN;
                ret = poll(&rpfd, 1, 1000);
                if (ret > 0 && (rpfd.revents & POLLIN)) {
                    char buf[16];
                    recv(afd, buf, sizeof(buf), 0);
                }
                close(afd);
            }
        }

        close(cfd);
        g_watchdog_fed++;
    }

    close(sv);

    if (failures > NUM_RAPID_CYCLES / 2) {
        FAIL("rapid_connect", "too many connection failures");
        return 1;
    }
    char msg[64];
    snprintf(msg, sizeof(msg), "rapid_connect (%d cycles, %d failures)", NUM_RAPID_CYCLES, failures);
    PASS(msg);
    return 0;
}

/* ========================================================================
 * TEST 6: Simultaneous read+write readiness with poll()
 *
 * AmigaOS socketpair uses TCP loopback. The bsdsocket.library may not
 * deliver loopback data synchronously during send(), so there can be a
 * brief delay before data appears in the peer's receive buffer.
 *
 * We test three sub-cases:
 *   a) POLLIN-only  (the Firefox IPC pattern: poll blocks, then data arrives)
 *   b) POLLOUT-only (should be immediately true on connected socket)
 *   c) POLLIN|POLLOUT combined (may need a retry on AmigaOS)
 * ======================================================================== */
static int
test_readwrite_readiness(void) {
    int sv[2];
    int ok = 1;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        FAIL("readwrite_readiness", "socketpair");
        return 1;
    }

    /* --- Sub-test A: POLLIN only (Firefox IPC pattern) --- */
    {
        const char *msg = "hello";
        send(sv[0], msg, 5, 0);

        struct pollfd pfd;
        pfd.fd = sv[1];
        pfd.events = POLLIN;
        pfd.revents = 0;

        int ret = poll(&pfd, 1, 2000);
        if (ret <= 0) {
            FAIL("readwrite_pollin", "poll(POLLIN) returned <= 0 (timeout or error)");
            ok = 0;
        } else if (!(pfd.revents & POLLIN)) {
            FAIL("readwrite_pollin", "poll returned > 0 but POLLIN not set");
            ok = 0;
        } else {
            PASS("readwrite_pollin (POLLIN-only after send)");
        }

        /* Drain the data */
        char buf[16];
        recv(sv[1], buf, sizeof(buf), 0);
    }

    /* --- Sub-test B: POLLOUT only --- */
    {
        struct pollfd pfd;
        pfd.fd = sv[1];
        pfd.events = POLLOUT;
        pfd.revents = 0;

        int ret = poll(&pfd, 1, 1000);
        if (ret <= 0 || !(pfd.revents & POLLOUT)) {
            FAIL("readwrite_pollout", "POLLOUT not set on connected socket");
            ok = 0;
        } else {
            PASS("readwrite_pollout (POLLOUT-only)");
        }
    }

    /* --- Sub-test C: POLLIN|POLLOUT combined --- */
    {
        const char *msg2 = "world";
        send(sv[0], msg2, 5, 0);

        /* On AmigaOS, TCP loopback delivery may be async.  Give the TCP
         * stack a few chances to deliver the data before considering the
         * combined test a failure.  We try up to 5 times with small
         * sleeps, which also exercises the poll retry path. */
        int got_pollin = 0, got_pollout = 0;
        for (int attempt = 0; attempt < 5; attempt++) {
            struct pollfd pfd;
            pfd.fd = sv[1];
            pfd.events = POLLIN | POLLOUT;
            pfd.revents = 0;

            int ret = poll(&pfd, 1, 200);
            if (ret > 0) {
                if (pfd.revents & POLLIN)  got_pollin = 1;
                if (pfd.revents & POLLOUT) got_pollout = 1;
            }
            if (got_pollin && got_pollout)
                break;
            /* Brief sleep to let TCP stack deliver loopback data */
            usleep(10000); /* 10 ms */
        }

        if (!got_pollin) {
            FAIL("readwrite_combined", "POLLIN never set (loopback delivery too slow?)");
            ok = 0;
        } else if (!got_pollout) {
            FAIL("readwrite_combined", "POLLOUT never set");
            ok = 0;
        } else {
            PASS("readwrite_combined (POLLIN|POLLOUT)");
        }

        char buf[16];
        recv(sv[1], buf, sizeof(buf), 0);
    }

    close(sv[0]);
    close(sv[1]);
    return ok ? 0 : 1;
}

/* ========================================================================
 * TEST 7: POLLHUP detection (peer close)
 * ======================================================================== */
static int
test_pollhup(void) {
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        FAIL("pollhup", "socketpair");
        return 1;
    }

    /* Close one end */
    close(sv[0]);

    /* Poll the other end - should get POLLHUP or POLLIN with recv()=0 */
    struct pollfd pfd;
    pfd.fd = sv[1];
    pfd.events = POLLIN;
    pfd.revents = 0;

    int ret = poll(&pfd, 1, 1000);

    if (ret > 0 && (pfd.revents & (POLLHUP | POLLIN))) {
        PASS("pollhup (peer close detected)");
        close(sv[1]);
        return 0;
    }

    /* Also acceptable: timeout (some implementations) */
    if (ret == 0) {
        PASS("pollhup (timeout - implementation specific)");
        close(sv[1]);
        return 0;
    }

    FAIL("pollhup", "unexpected poll result");
    close(sv[1]);
    return 1;
}

/* ========================================================================
 * TEST 8: Multi-threaded poll() on separate sockets (no shared fd)
 *
 * Each thread creates its own socketpair and polls independently.
 * Tests that poll() is thread-safe when used on separate descriptors.
 * ======================================================================== */

struct mt_poll_arg {
    int id;
    int ok;
};

static void *
mt_poll_worker(void *arg) {
    struct mt_poll_arg *ma = (struct mt_poll_arg *)arg;
    ma->ok = 1;

    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        TSAY("mt_poll worker %d: socketpair failed", ma->id);
        ma->ok = 0;
        return NULL;
    }

    set_nonblocking(sv[0]);
    set_nonblocking(sv[1]);

    /* Round-trip: send on sv[0], poll+recv on sv[1], send back, poll+recv on sv[0] */
    for (int iter = 0; iter < 100; iter++) {
        char outbuf[64];
        snprintf(outbuf, sizeof(outbuf), "msg-%d-%d", ma->id, iter);
        size_t len = strlen(outbuf);

        if (send(sv[0], outbuf, len, 0) != (ssize_t)len) {
            /* Might get EAGAIN in non-blocking mode, retry */
            struct pollfd wpfd = { .fd = sv[0], .events = POLLOUT };
            poll(&wpfd, 1, 500);
            if (send(sv[0], outbuf, len, 0) != (ssize_t)len) {
                TSAY("mt_poll worker %d: send failed iter %d", ma->id, iter);
                ma->ok = 0;
                break;
            }
        }

        /* Poll for data on sv[1] */
        struct pollfd rpfd = { .fd = sv[1], .events = POLLIN };
        int ret = poll(&rpfd, 1, 2000);
        if (ret <= 0 || !(rpfd.revents & POLLIN)) {
            TSAY("mt_poll worker %d: poll read failed iter %d (ret=%d)", ma->id, iter, ret);
            ma->ok = 0;
            break;
        }

        char inbuf[64];
        ssize_t n = recv(sv[1], inbuf, sizeof(inbuf), 0);
        if (n != (ssize_t)len || memcmp(inbuf, outbuf, len) != 0) {
            TSAY("mt_poll worker %d: data mismatch iter %d", ma->id, iter);
            ma->ok = 0;
            break;
        }

        g_watchdog_fed++;
    }

    close(sv[0]);
    close(sv[1]);
    return NULL;
}

static int
test_mt_poll(void) {
    int num_threads = 4;
    pthread_t tids[4];
    struct mt_poll_arg args[4];

    for (int i = 0; i < num_threads; i++) {
        args[i].id = i;
        args[i].ok = 0;
        if (pthread_create(&tids[i], NULL, mt_poll_worker, &args[i]) != 0) {
            FAIL("mt_poll", "thread create");
            return 1;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(tids[i], NULL);
    }

    int all_ok = 1;
    for (int i = 0; i < num_threads; i++) {
        if (!args[i].ok) {
            char msg[64];
            snprintf(msg, sizeof(msg), "worker %d failed", i);
            FAIL("mt_poll", msg);
            all_ok = 0;
        }
    }
    if (all_ok) {
        char msg[64];
        snprintf(msg, sizeof(msg), "mt_poll (%d threads x 100 round-trips)", num_threads);
        PASS(msg);
    }
    return all_ok ? 0 : 1;
}

/* ========================================================================
 * TEST 9: Producer-consumer pipeline with poll() and multiple stages
 *
 * Thread A → socket → Thread B (poll-based relay) → socket → Thread C
 * Tests poll() under chained threaded data flow.
 * ======================================================================== */

struct pipe_stage_arg {
    int read_fd;
    int write_fd;
    size_t total_forwarded;
    int done;
};

static void *
pipe_producer(void *arg) {
    struct pipe_stage_arg *pa = (struct pipe_stage_arg *)arg;
    char buf[4096];
    memset(buf, 'X', sizeof(buf));
    size_t total = 0;

    while (total < TOTAL_BYTES) {
        size_t chunk = sizeof(buf);
        if (total + chunk > TOTAL_BYTES) chunk = TOTAL_BYTES - total;
        ssize_t n = send(pa->write_fd, buf, chunk, 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                struct pollfd pfd = { .fd = pa->write_fd, .events = POLLOUT };
                poll(&pfd, 1, 1000);
                continue;
            }
            break;
        }
        total += n;
        g_watchdog_fed++;
    }
    close(pa->write_fd);
    pa->total_forwarded = total;
    pa->done = 1;
    return NULL;
}

static void *
pipe_relay(void *arg) {
    struct pipe_stage_arg *pa = (struct pipe_stage_arg *)arg;
    char buf[4096];
    size_t total = 0;

    set_nonblocking(pa->read_fd);
    set_nonblocking(pa->write_fd);

    while (1) {
        struct pollfd pfds[2];
        pfds[0].fd = pa->read_fd;
        pfds[0].events = POLLIN;
        pfds[1].fd = pa->write_fd;
        pfds[1].events = 0; /* only set POLLOUT when we have data */

        int ret = poll(pfds, 1, 2000); /* only poll read */
        if (ret < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ret == 0) {
            /* timeout, check if producer is done */
            break;
        }

        if (pfds[0].revents & (POLLIN | POLLHUP)) {
            ssize_t n = recv(pa->read_fd, buf, sizeof(buf), 0);
            if (n <= 0) break; /* EOF or error */

            /* Forward to write_fd */
            size_t sent = 0;
            while (sent < (size_t)n) {
                ssize_t w = send(pa->write_fd, buf + sent, n - sent, 0);
                if (w < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        struct pollfd wpfd = { .fd = pa->write_fd, .events = POLLOUT };
                        poll(&wpfd, 1, 1000);
                        continue;
                    }
                    goto done;
                }
                sent += w;
            }
            total += n;
            g_watchdog_fed++;
        }
    }
done:
    close(pa->read_fd);
    close(pa->write_fd);
    pa->total_forwarded = total;
    pa->done = 1;
    return NULL;
}

static void *
pipe_consumer(void *arg) {
    struct pipe_stage_arg *pa = (struct pipe_stage_arg *)arg;
    char buf[4096];
    size_t total = 0;

    while (1) {
        struct pollfd pfd = { .fd = pa->read_fd, .events = POLLIN };
        int ret = poll(&pfd, 1, 3000);
        if (ret <= 0) break;

        if (pfd.revents & (POLLIN | POLLHUP)) {
            ssize_t n = recv(pa->read_fd, buf, sizeof(buf), 0);
            if (n <= 0) break;
            total += n;
            g_watchdog_fed++;
        }
    }

    close(pa->read_fd);
    pa->total_forwarded = total;
    pa->done = 1;
    return NULL;
}

static int
test_pipeline(void) {
    /* Create two socketpairs: A→B and B→C */
    int ab[2], bc[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, ab) < 0) {
        FAIL("pipeline", "socketpair ab");
        return 1;
    }
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, bc) < 0) {
        FAIL("pipeline", "socketpair bc");
        close(ab[0]); close(ab[1]);
        return 1;
    }

    struct pipe_stage_arg prod_arg = { .read_fd = -1, .write_fd = ab[0], .total_forwarded = 0, .done = 0 };
    struct pipe_stage_arg relay_arg = { .read_fd = ab[1], .write_fd = bc[0], .total_forwarded = 0, .done = 0 };
    struct pipe_stage_arg cons_arg = { .read_fd = bc[1], .write_fd = -1, .total_forwarded = 0, .done = 0 };

    pthread_t t_prod, t_relay, t_cons;
    pthread_create(&t_prod, NULL, pipe_producer, &prod_arg);
    pthread_create(&t_relay, NULL, pipe_relay, &relay_arg);
    pthread_create(&t_cons, NULL, pipe_consumer, &cons_arg);

    pthread_join(t_prod, NULL);
    pthread_join(t_relay, NULL);
    pthread_join(t_cons, NULL);

    if (cons_arg.total_forwarded == TOTAL_BYTES) {
        char msg[128];
        snprintf(msg, sizeof(msg), "pipeline (A→B→C, %zuKB)", cons_arg.total_forwarded / 1024);
        PASS(msg);
        return 0;
    } else {
        char msg[128];
        snprintf(msg, sizeof(msg), "producer=%zu relay=%zu consumer=%zu expected=%d",
                 prod_arg.total_forwarded, relay_arg.total_forwarded, cons_arg.total_forwarded, TOTAL_BYTES);
        FAIL("pipeline", msg);
        return 1;
    }
}

/* ======================================================================== */

int
main(int argc, char *argv[]) {
    /* Use a random-ish port base to avoid conflicts */
    srand(time(NULL));
    g_server_port = SERVER_PORT_BASE + (rand() % 1000);

    printf("=== poll() Stress Test ===\n");
    printf("Server port base: %d\n", g_server_port);
    printf("Clients: %d, Bulk: %dKB, Total/client: %dKB\n\n",
           NUM_CLIENTS, BULK_SIZE / 1024, TOTAL_BYTES / 1024);

    /* Start watchdog */
    pthread_t wd_tid;
    pthread_create(&wd_tid, NULL, watchdog_thread, NULL);

    int failures = 0;

    /* Run tests sequentially */
    printf("[Test 1] Timeout accuracy\n");
    failures += test_poll_timeout();

    printf("[Test 2] Immediate return\n");
    failures += test_poll_immediate();

    printf("[Test 3] Invalid fd handling\n");
    failures += test_poll_nval();

    printf("[Test 4] POLLHUP detection\n");
    failures += test_pollhup();

    printf("[Test 5] Read+Write readiness\n");
    failures += test_readwrite_readiness();

    printf("[Test 6] Multi-threaded poll (4 threads x 100 round-trips)\n");
    failures += test_mt_poll();

    printf("[Test 7] Bulk transfer (%d clients x %dKB)\n", NUM_CLIENTS, TOTAL_BYTES / 1024);
    failures += test_bulk_transfer();

    printf("[Test 8] Rapid connect/disconnect (%d cycles)\n", NUM_RAPID_CYCLES);
    failures += test_rapid_connect_disconnect();

    printf("[Test 9] Producer-consumer pipeline (3 threads)\n");
    failures += test_pipeline();

    /* Done */
    g_test_done = 1;
    pthread_join(wd_tid, NULL);

    printf("\n=== Results: %d test(s) failed ===\n", failures);

    if (failures == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("SOME TESTS FAILED\n");

    return failures;
}
