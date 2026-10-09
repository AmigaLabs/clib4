/*
 * ucontext.c - the user-context functions, for ARM.
 *
 * The PowerPC ones are assembly in ucontext/ and save the PowerPC register file.
 * The ARM register file and its switch are not ported yet, so these four return
 * ENOSYS instead of switching. They exist so clib4.library links and a program
 * that calls them gets an error, not a missing symbol.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <errno.h>
#include <ucontext.h>

int
getcontext(ucontext_t *ucp)
{
    (void) ucp;
    errno = ENOSYS;
    return -1;
}

int
setcontext(const ucontext_t *ucp)
{
    (void) ucp;
    errno = ENOSYS;
    return -1;
}

int
swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
    (void) oucp;
    (void) ucp;
    errno = ENOSYS;
    return -1;
}

void
makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
    (void) ucp;
    (void) func;
    (void) argc;
    errno = ENOSYS;
}
