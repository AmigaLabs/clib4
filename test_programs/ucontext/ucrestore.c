/*
 * Debug: verifica che i registri ripristinati all'ingresso della funzione
 * target corrispondano a quelli scritti da makecontext.
 * Stampa r3/r4 visti dalla funzione E i gregs letti dalla struttura.
 */
#include <stdio.h>
#include <ucontext.h>

static char stack[16384];
static ucontext_t ctx, main_ctx;

/* Legge r3 e r4 così come arrivano all'ingresso */
static void target(int a, int b) {
    printf("target: r3(arg0)=%d r4(arg1)=%d\n", a, b);
    printf("target: gregs[3] in memoria = %lu\n", ctx.uc_mcontext.gregs[3]);
    printf("target: gregs[4] in memoria = %lu\n", ctx.uc_mcontext.gregs[4]);
    setcontext(&main_ctx);
}

int main(void) {
    getcontext(&ctx);
    ctx.uc_stack.ss_sp = stack;
    ctx.uc_stack.ss_size = sizeof(stack);
    ctx.uc_link = &main_ctx;
    makecontext(&ctx, (void (*)(void)) target, 2, 200, 300);

    printf("main: gregs[3]=%lu gregs[4]=%lu (attesi 200/300)\n",
           ctx.uc_mcontext.gregs[3], ctx.uc_mcontext.gregs[4]);

    swapcontext(&main_ctx, &ctx);

    printf("main: ritornato\n");
    return 0;
}
