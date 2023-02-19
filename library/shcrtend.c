/*
 * $Id: shcrtend.c,v 1.1 2023-02-18 19:35:03 clib2devs Exp $
 */

typedef void (*func_ptr)(void);

static func_ptr __CTOR_END__[1] __attribute__(( used, section(".ctors"), aligned(sizeof(func_ptr)))) = {(func_ptr) 0};
static func_ptr __DTOR_END__[1] __attribute__(( used, section(".dtors"), aligned(sizeof(func_ptr)))) = {(func_ptr) 0};
