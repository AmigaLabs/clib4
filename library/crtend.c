/*
 * $Id: crtend.c,v 1.4 2026-03-11 21:07:25 clib4devs Exp $
  */

static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"))) = { (void *)0 };
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"))) = { (void *)0 };

/* Mark the end of the exception handling frames */
static const char __EH_FRAME_END__[]
    __attribute__((used, section(".eh_frame"), aligned(4)))
    = { 0, 0, 0, 0 };