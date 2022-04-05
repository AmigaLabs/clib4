/* Wheel-of-Fortune Memory Allocator
 * Copyright 2013, Evan Huus <eapache@gmail.com>
 */

#ifndef __WOF_ALLOCATOR_H__
#define __WOF_ALLOCATOR_H__

#include <features.h>
#include <string.h>

__BEGIN_DECLS

typedef struct _wof_allocator_t wof_allocator_t;

extern void *wof_alloc(wof_allocator_t *allocator, const size_t size);
extern void  wof_free(wof_allocator_t *allocator, void *ptr);
extern void *wof_realloc(wof_allocator_t *allocator, void *ptr, const size_t size);
extern void  wof_free_all(wof_allocator_t *allocator);
extern void  wof_gc(wof_allocator_t *allocator);
extern void  wof_allocator_destroy(wof_allocator_t *allocator);
extern wof_allocator_t *wof_allocator_new(void);

__END_DECLS

#endif /* __WOF_ALLOCATOR_H__ */