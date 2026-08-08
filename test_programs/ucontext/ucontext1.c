/*
 * Test for the ucontext.h API (getcontext/setcontext/swapcontext/makecontext)
 *
 * Two coroutine-style contexts ping-pong between each other and then
 * return to main via uc_link.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 16384

static ucontext_t ctx_main, ctx_func1, ctx_func2;
static char stack1[STACK_SIZE];
static char stack2[STACK_SIZE];

static int errors = 0;
static int sequence = 0;

static void
check(const char *what, int expected) {
    sequence++;
    if (sequence != expected) {
        printf("FAIL: %s executed out of order (got step %d, expected %d)\n", what, sequence, expected);
        errors++;
    } else {
        printf("ok: %s (step %d)\n", what, sequence);
    }
}

static void
func1(int arg) {
    if (arg != 100) {
        printf("FAIL: func1 got arg %d, expected 100\n", arg);
        errors++;
    }
    check("func1: started", 1);
    swapcontext(&ctx_func1, &ctx_func2);
    check("func1: resumed", 3);
    /* returns to ctx_func2 via uc_link */
}

static void
func2(int arg1, int arg2) {
    if (arg1 != 200 || arg2 != 300) {
        printf("FAIL: func2 got args %d/%d, expected 200/300\n", arg1, arg2);
        errors++;
    }
    check("func2: started", 2);
    swapcontext(&ctx_func2, &ctx_func1);
    check("func2: resumed", 4);
    /* returns to ctx_main via uc_link */
}

int
main(void) {
    /* getcontext must return 0 */
    if (getcontext(&ctx_func1) != 0) {
        printf("FAIL: getcontext returned non-zero\n");
        return EXIT_FAILURE;
    }
    ctx_func1.uc_stack.ss_sp = stack1;
    ctx_func1.uc_stack.ss_size = sizeof(stack1);
    ctx_func1.uc_link = &ctx_func2;
    makecontext(&ctx_func1, (void (*)(void)) func1, 1, 100);

    if (getcontext(&ctx_func2) != 0) {
        printf("FAIL: getcontext returned non-zero\n");
        return EXIT_FAILURE;
    }
    ctx_func2.uc_stack.ss_sp = stack2;
    ctx_func2.uc_stack.ss_size = sizeof(stack2);
    ctx_func2.uc_link = &ctx_main;
    makecontext(&ctx_func2, (void (*)(void)) func2, 2, 200, 300);

    /* Start the ping-pong: main -> func1 -> func2 -> func1 -> func2 ->
     * func1 returns -> func2 (via uc_link) -> main (via uc_link) */
    swapcontext(&ctx_main, &ctx_func1);
    check("main: back from coroutines", 5);

    if (errors > 0) {
        printf("FAILURE: %d errors detected\n", errors);
        return EXIT_FAILURE;
    }

    printf("SUCCESS\n");
    return EXIT_SUCCESS;
}
