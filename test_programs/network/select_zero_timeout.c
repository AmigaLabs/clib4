/*
 * select() with a zero timeout must poll once and return straight away,
 * also when stdin is redirected and no descriptor is ready (issue #490).
 *
 * Run as:  select_zero_timeout <NIL:
 *          select_zero_timeout <somefile
 *          select_zero_timeout 1 <NIL:     (1 second timeout instead)
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    fd_set rfds;
    struct timeval tv = { 0, 0 };
    struct timeval start, end;
    long elapsed_ms;
    int n;

    if (argc > 1)
        tv.tv_sec = atol(argv[1]);

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    gettimeofday(&start, NULL);
    n = select(1, &rfds, NULL, NULL, &tv);
    gettimeofday(&end, NULL);

    elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    printf("select returned %d, stdin %s, took %ld ms\n", n, FD_ISSET(0, &rfds) ? "set" : "clear", elapsed_ms);

    return 0;
}
