/*
 * $Id: crtend.c,v 1.3 2023-02-18 21:07:25 clib2devs Exp $
  */

typedef void (*func_ptr)(void);

static func_ptr __CTOR_END__[1] __attribute__(( used, section(".ctors"), aligned(sizeof(func_ptr)))) = {(func_ptr) 0};
static func_ptr __DTOR_END__[1] __attribute__(( used, section(".dtors"), aligned(sizeof(func_ptr)))) = {(func_ptr) 0};
