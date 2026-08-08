/*
 * Debug: dump the gregs that makecontext() sets up for a context.
 * gregs[3]=arg0, gregs[4]=arg1, gregs[1]=sp, gregs[32]=nip, gregs[36]=lnk
 */
#include <stdio.h>
#include <ucontext.h>

static char stack[16384];
static ucontext_t ctx;

static void target(int a, int b) {
    printf("target: a=%d b=%d\n", a, b);
}

int main(void) {
    getcontext(&ctx);
    ctx.uc_stack.ss_sp = stack;
    ctx.uc_stack.ss_size = sizeof(stack);
    ctx.uc_link = NULL;
    makecontext(&ctx, (void (*)(void)) target, 2, 200, 300);

    printf("sizeof(ucontext_t) = %u\n", (unsigned)sizeof(ucontext_t));
    printf("gregs[1]  (sp)  = %lu\n", ctx.uc_mcontext.gregs[1]);
    printf("gregs[3]  (arg0)= %lu\n", ctx.uc_mcontext.gregs[3]);
    printf("gregs[4]  (arg1)= %lu\n", ctx.uc_mcontext.gregs[4]);
    printf("gregs[31] (link)= %lu\n", ctx.uc_mcontext.gregs[31]);
    printf("gregs[32] (nip) = %lu\n", ctx.uc_mcontext.gregs[32]);
    printf("gregs[36] (lnk) = %lu\n", ctx.uc_mcontext.gregs[36]);

    if (ctx.uc_mcontext.gregs[3] == 200 && ctx.uc_mcontext.gregs[4] == 300)
        printf("GREGS-OK\n");
    else
        printf("GREGS-BAD\n");
    return 0;
}
