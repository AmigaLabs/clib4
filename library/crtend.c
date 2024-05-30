/*
 * $Id: crtend.c,v 1.3 2023-02-18 21:07:25 clib4devs Exp $
  */

static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"))) = { (void *)0 };
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"))) = { (void *)0 };
