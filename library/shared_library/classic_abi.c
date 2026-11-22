/*
 * $Id: classic_abi.c,v 1.0 2026-11-22 12:00:00 clib4devs Exp $
 *
 * Classic ABI entry points of the SPE clib4.library: where to find the
 * classic floating point registers.  See classic_abi.h.
 */

#include <exec/types.h>
#include <stddef.h>

#include "classic_abi.h"

_Static_assert(offsetof(struct ExceptionContext, fpr) == CLASSIC_CONTEXT_FPR,
               "struct ExceptionContext's fpr isn't where classic_abi.h expects it");

int __classic_fprs_direct;

/* The classic caller's fN, n = 1-8, with stfd */
double
__classic_stfd(int n) {
    double d = 0;

    switch (n) {
        case 1: __CLASSIC_STFD(1, &d); break;
        case 2: __CLASSIC_STFD(2, &d); break;
        case 3: __CLASSIC_STFD(3, &d); break;
        case 4: __CLASSIC_STFD(4, &d); break;
        case 5: __CLASSIC_STFD(5, &d); break;
        case 6: __CLASSIC_STFD(6, &d); break;
        case 7: __CLASSIC_STFD(7, &d); break;
        case 8: __CLASSIC_STFD(8, &d); break;
    }
    return d;
}

/* Return d in the classic caller's f1, with lfd */
void
__classic_lfd_f1(double d) {
    __CLASSIC_LFD(1, &d);
}

/*
 * Check that SPRG7 leads to the running task's emulated floating point
 * registers, both ways: a value the kernel's emulation of lfd puts in f0
 * shows up where the entry points look, and one they write there comes
 * out of stfd.  f0 is volatile, so the task that opens the library
 * doesn't mind.
 */
void
__classic_abi_init(void) {
    static const union { uint64 u; double d; } in = { 0x3ff0123456789abcULL }, out = { 0xc00fedcba9876543ULL };
    union { uint64 u; double d; } value;
    struct ExceptionContext *context;

    __classic_fprs_direct = 0;

    __CLASSIC_LFD(0, &in.d);
    context = __classic_context();
    if (context == NULL)
        return;

    value.d = __classic_get_fpr(context, 0);
    if (value.u != in.u)
        return;

    __classic_set_fpr(context, 0, out.d);
    value.u = 0;
    __CLASSIC_STFD(0, &value.d);
    __classic_fprs_direct = (value.u == out.u);
}
