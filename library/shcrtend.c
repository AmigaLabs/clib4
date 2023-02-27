/*
 * $Id: shcrtend.c,v 1.1 2023-02-18 19:35:03 clib2devs Exp $
 */

static void (*__CTOR_END__[1])(void) __attribute__((used, section(".ctors")));
static void (*__DTOR_END__[1])(void) __attribute__((used, section(".dtors")));
