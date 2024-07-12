/* Wheel-of-Fortune Memory Allocator
 * Copyright 2013, Evan Huus <eapache@gmail.com>
 */

#ifndef __WOF_ALLOCATOR_H__
#define __WOF_ALLOCATOR_H__

#include <features.h>
#include <string.h>

/* https://mail.gnome.org/archives/gtk-devel-list/2004-December/msg00091.html
 * The 2*sizeof(size_t) alignment here is borrowed from GNU libc, so it should
 * be good most everywhere. It is more conservative than is needed on some
 * 64-bit platforms, but ia64 does require a 16-byte alignment. The SIMD
 * extensions for x86 and ppc32 would want a larger alignment than this, but
 * we don't need to do better than malloc.
 */
#define WOF_ALIGN_AMOUNT (2 * sizeof (size_t))
#define WOF_ALIGN_SIZE(SIZE) ((~(WOF_ALIGN_AMOUNT-1)) & \
        ((SIZE) + (WOF_ALIGN_AMOUNT-1)))

/* When required, allocate more memory from the OS in chunks of this size.
 * 8MB is a pretty arbitrary value - it's big enough that it should last a while
 * and small enough that a mostly-unused one doesn't waste *too* much. It's
 * also a nice power of two, of course. */
#define WOF_BLOCK_SIZE (8 * 1024 * 1024)

/* The header for an entire OS-level 'block' of memory */
typedef struct _wof_block_hdr_t {
    struct _wof_block_hdr_t *prev, *next;
} wof_block_hdr_t;

/* The header for a single 'chunk' of memory as returned from alloc/realloc.
 * The 'jumbo' flag indicates an allocation larger than a normal-sized block
 * would be capable of serving. If this is set, it is the only chunk in the
 * block and the other chunk header fields are irrelevant.
 */
typedef struct _wof_chunk_hdr_t {
    int prev;

    /* flags */
    int last: 1;
    int used: 1;
    int jumbo: 1;

    int len: 29;
} wof_chunk_hdr_t;

/* Handy macros for navigating the chunks in a block as if they were a
 * doubly-linked list. */
#define WOF_CHUNK_PREV(CHUNK) ((CHUNK)->prev \
        ? ((wof_chunk_hdr_t*)(((unsigned char*)(CHUNK)) - (CHUNK)->prev)) \
        : NULL)

#define WOF_CHUNK_NEXT(CHUNK) ((CHUNK)->last \
        ? NULL \
        : ((wof_chunk_hdr_t*)(((unsigned char*)(CHUNK)) + (CHUNK)->len)))

#define WOF_CHUNK_HEADER_SIZE WOF_ALIGN_SIZE(sizeof(wof_chunk_hdr_t))

/* other handy chunk macros */
#define WOF_CHUNK_TO_DATA(CHUNK)  ((void*)((unsigned char*)(CHUNK) + WOF_CHUNK_HEADER_SIZE))
#define WOF_DATA_TO_CHUNK(DATA)   ((wof_chunk_hdr_t*)((unsigned char*)(DATA) - WOF_CHUNK_HEADER_SIZE))
#define WOF_CHUNK_DATA_LEN(CHUNK) ((CHUNK)->len - WOF_CHUNK_HEADER_SIZE)

/* some handy block macros */
#define WOF_BLOCK_HEADER_SIZE     WOF_ALIGN_SIZE(sizeof(wof_block_hdr_t))
#define WOF_BLOCK_TO_CHUNK(BLOCK) ((wof_chunk_hdr_t*)((unsigned char*)(BLOCK) + WOF_BLOCK_HEADER_SIZE))
#define WOF_CHUNK_TO_BLOCK(CHUNK) ((wof_block_hdr_t*)((unsigned char*)(CHUNK) - WOF_BLOCK_HEADER_SIZE))

#define WOF_BLOCK_MAX_ALLOC_SIZE (WOF_BLOCK_SIZE - (WOF_BLOCK_HEADER_SIZE + WOF_CHUNK_HEADER_SIZE))

/* This is what the 'data' section of a chunk contains if it is free. */
typedef struct _wof_free_hdr_t {
    wof_chunk_hdr_t *prev, *next;
} wof_free_hdr_t;

/* Handy macro for accessing the free-header of a chunk */
#define WOF_GET_FREE(CHUNK) ((wof_free_hdr_t*) WOF_CHUNK_TO_DATA(CHUNK))

/* The size of the free header does not need to be aligned, as it is never used
 * in any way that would affect the alignment of the memory returned from
 * wof_alloc. This macro is defined just for consistency with all the other
 * WOF_*_SIZE macros (which do need to be aligned). */
#define WOF_FREE_HEADER_SIZE sizeof(wof_free_hdr_t)

struct _wof_allocator_t {
    wof_block_hdr_t *block_list;
    wof_chunk_hdr_t *master_head;
    wof_chunk_hdr_t *recycler_head;
    struct SignalSemaphore *semaphore;
};

__BEGIN_DECLS

typedef struct _wof_allocator_t wof_allocator_t;

extern void *wof_alloc(wof_allocator_t * allocator, const size_t size);
extern void wof_free(wof_allocator_t * allocator, void * ptr);
extern void *wof_realloc(wof_allocator_t * allocator, void * ptr, const size_t size);
extern void wof_free_all(wof_allocator_t * allocator);
extern void wof_gc(wof_allocator_t * allocator);
extern void wof_allocator_destroy(wof_allocator_t * allocator);

extern wof_allocator_t *wof_allocator_new(void);

__END_DECLS

#endif /* __WOF_ALLOCATOR_H__ */