/*
 * $Id: crtend.c,v 1.2 2005-03-09 21:07:25 clib2devs Exp $
  */

static void (*__CTOR_LIST__[1]) (void) __attribute__((used, section(".ctors"), aligned(sizeof(void (*)(void))) ));
static void (*__DTOR_LIST__[1]) (void) __attribute__((used, section(".dtors"), aligned(sizeof(void (*)(void))) ));
