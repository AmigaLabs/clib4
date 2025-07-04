/* ----------------------------------------------------------------------------
Copyright (c) 2018-2023, Microsoft Research, Daan Leijen
This is free software; you can redistribute it and/or modify it under the
terms of the MIT license. A copy of the license can be found in the file
"LICENSE" at the root of this distribution.
-----------------------------------------------------------------------------*/

#include "os4_prim.c" // emmalloc_* (AmigaOS4)

#define mi_attr_constructor __attribute__((constructor))
#define mi_attr_destructor  __attribute__((destructor))

static void mi_attr_constructor mi_process_attach(void) {
    _mi_auto_process_init();
}

static void mi_attr_destructor mi_process_detach(void) {
    _mi_auto_process_done();
}

// Generic allocator init/done callback
#ifndef MI_PRIM_HAS_ALLOCATOR_INIT
bool _mi_is_redirected(void) {
    return false;
}

bool _mi_allocator_init(const char **message) {
    if (message != NULL) {
        *message = NULL;
    }
    return true;
}


void _mi_allocator_done(void) {
    // nothing to do
}
#endif
