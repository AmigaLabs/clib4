/*
 * $Id: crtend.c,v 1.3 2023-02-18 21:07:25 clib4devs Exp $
 * Modified to add EH frame support for C++ exceptions
  */

static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"))) = { (void *)0 };
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"))) = { (void *)0 };

/* Mark the end of the exception handling frames */
static const char __EH_FRAME_END__[]
    __attribute__((used, section(".eh_frame"), aligned(4)))
    = { 0, 0, 0, 0 };
