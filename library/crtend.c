/*
 * $Id: crtend.c,v 1.3 2023-02-18 21:07:25 clib2devs Exp $
  */

static void (*__CTOR_LIST__[1])(void) __attribute__((used,section(".ctors")));
static void (*__DTOR_LIST__[1])(void) __attribute__((used,section(".dtors")));
