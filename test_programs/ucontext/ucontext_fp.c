/*
 * Test FP register preservation across ucontext switches.
 *
 * The ucontext implementation must save/restore the non-volatile FPRs
 * (f14-f31) and the FPSCR. This test:
 *   - context A (main): loads f14-f31 with known values, swaps to B
 *   - context B (worker): loads f14-f31 with DIFFERENT values, swaps back
 *   - A reads back f14-f31 and verifies they still hold its own values
 *   - B does the same when resumed
 *
 * The values are loaded/stored with inline asm so the compiler cannot
 * keep them out of the real FP registers.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ucontext.h>

#define NFPR 18  /* f14 .. f31 */

static ucontext_t ctx_main, ctx_worker;
static char stack_worker[16384];
static int errors = 0;

/* Distinct, exactly-representable values */
static double vals_a[NFPR];
static double vals_b[NFPR];
static double readback[NFPR];

static void
load_fprs(const double *v) {
    __asm__ volatile(
        "lfd 14,   0(%0)\n"
        "lfd 15,   8(%0)\n"
        "lfd 16,  16(%0)\n"
        "lfd 17,  24(%0)\n"
        "lfd 18,  32(%0)\n"
        "lfd 19,  40(%0)\n"
        "lfd 20,  48(%0)\n"
        "lfd 21,  56(%0)\n"
        "lfd 22,  64(%0)\n"
        "lfd 23,  72(%0)\n"
        "lfd 24,  80(%0)\n"
        "lfd 25,  88(%0)\n"
        "lfd 26,  96(%0)\n"
        "lfd 27, 104(%0)\n"
        "lfd 28, 112(%0)\n"
        "lfd 29, 120(%0)\n"
        "lfd 30, 128(%0)\n"
        "lfd 31, 136(%0)\n"
        :
        : "b"(v));
}

static void
store_fprs(double *v) {
    __asm__ volatile(
        "stfd 14,   0(%0)\n"
        "stfd 15,   8(%0)\n"
        "stfd 16,  16(%0)\n"
        "stfd 17,  24(%0)\n"
        "stfd 18,  32(%0)\n"
        "stfd 19,  40(%0)\n"
        "stfd 20,  48(%0)\n"
        "stfd 21,  56(%0)\n"
        "stfd 22,  64(%0)\n"
        "stfd 23,  72(%0)\n"
        "stfd 24,  80(%0)\n"
        "stfd 25,  88(%0)\n"
        "stfd 26,  96(%0)\n"
        "stfd 27, 104(%0)\n"
        "stfd 28, 112(%0)\n"
        "stfd 29, 120(%0)\n"
        "stfd 30, 128(%0)\n"
        "stfd 31, 136(%0)\n"
        :
        : "b"(v));
}

static void
check_fprs(const char *who, const double *expected) {
    store_fprs(readback);
    for (int i = 0; i < NFPR; i++) {
        if (memcmp(&readback[i], &expected[i], sizeof(double)) != 0) {
            printf("FAIL: %s: f%d = %f, expected %f\n", who, i + 14, readback[i], expected[i]);
            errors++;
        }
    }
    if (errors == 0)
        printf("ok: %s: f14-f31 preserved\n", who);
}

static void
worker(void) {
    /* Clobber the FPRs with worker's own values */
    load_fprs(vals_b);
    /* Give control back to main, which will verify its own values */
    swapcontext(&ctx_worker, &ctx_main);

    /* Resumed: main swapped back. Verify OUR values survived. */
    check_fprs("worker", vals_b);
    /* Return to main via uc_link */
}

int
main(void) {
    for (int i = 0; i < NFPR; i++) {
        vals_a[i] = 1.5 + i;          /* 1.5, 2.5, 3.5, ... */
        vals_b[i] = -1000.0 - i * 4;  /* large negative, distinct */
    }

    getcontext(&ctx_worker);
    ctx_worker.uc_stack.ss_sp = stack_worker;
    ctx_worker.uc_stack.ss_size = sizeof(stack_worker);
    ctx_worker.uc_link = &ctx_main;
    makecontext(&ctx_worker, worker, 0);

    /* Load OUR values, then switch to the worker which clobbers them */
    load_fprs(vals_a);
    swapcontext(&ctx_main, &ctx_worker);

    /* Resumed: verify our values survived the worker's clobbering */
    check_fprs("main", vals_a);

    /* Let the worker verify its own values, then come back via uc_link */
    swapcontext(&ctx_main, &ctx_worker);

    if (errors > 0) {
        printf("FAILURE: %d errors detected\n", errors);
        return EXIT_FAILURE;
    }

    printf("SUCCESS\n");
    return EXIT_SUCCESS;
}
