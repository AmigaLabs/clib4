/* wmem_allocator_simple.c
 * Wireshark Memory Manager Simple Allocator
 * Copyright 2012, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include "wmem_core.h"
#include "wmem_allocator.h"
#include "wmem_allocator_simple.h"

#define DEFAULT_ALLOCS 8192

typedef struct _wmem_simple_allocator_t {
    int size;
    int count;
    void **ptrs;
    size_t *sizes; // we need this for realloc on amigaos
} wmem_simple_allocator_t;

#if MEMORY_DEBUG
void dump_ptrs(const char *function, void *private_data);
void dump_ptrs(const char *function, void *private_data) {
    wmem_simple_allocator_t *allocator;
    allocator = (wmem_simple_allocator_t *) private_data;

    for (int i = 0; i < allocator->count; i++)
        D(("[%s :] %ld : ptr [0x%lx] size [0x%lx]\n", function, i, allocator->ptrs[i], allocator->sizes[i]));
}
#endif

static void *
wmem_simple_alloc(void *private_data, const size_t size) {
    wmem_simple_allocator_t *allocator;

    allocator = (wmem_simple_allocator_t *) private_data;

    if (__clib4_unlikely(allocator->count == allocator->size)) {

#if MEMORY_DEBUG
    D(("[wmem_simple_alloc :] Alloc : size [0x%lx]\n", size));
#endif

    if (__clib4_unlikely(allocator->count == allocator->size)) {

#if MEMORY_DEBUG
        D(("[wmem_simple_alloc :] Growing ptrs array.\n"));
#endif

        int old_size = allocator->size;
        allocator->size *= 2;

        void **new_ptrs = (void **) wmem_alloc(NULL, sizeof(void *) * allocator->size);
        memcpy(new_ptrs, allocator->ptrs, sizeof(void *) * old_size);
        wmem_free(NULL, allocator->ptrs);
        allocator->ptrs = new_ptrs;

        // allocator->ptrs = (void **) wmem_realloc(NULL, allocator->ptrs,
        //                                          sizeof(void *) * allocator->size);

        size_t *new_sizes = (size_t *) wmem_alloc(NULL, sizeof(size_t) * allocator->size);
        memcpy(new_sizes, allocator->sizes, sizeof(size_t) * old_size);
        wmem_free(NULL, allocator->sizes);
        allocator->sizes = new_sizes;
    }

    allocator->sizes[allocator->count] = size;
    allocator->ptrs[allocator->count] = wmem_alloc(NULL, size);
    allocator->count++;

    return allocator->ptrs[allocator->count-1];
#if MEMORY_DEBUG
    D(("[wmem_simple_alloc :] count [%d] ptr [0x%lx] size [0x%lx].\n", allocator->count, allocator->ptrs[allocator->count], allocator->sizes[allocator->count]));
#endif
    allocator->count++;

#if MEMORY_DEBUG
    dump_ptrs("simple_alloc", private_data);
#endif

    return allocator->ptrs[allocator->count - 1];
}

static void
wmem_simple_free(void *private_data, void *ptr) {
    int i;
    wmem_simple_allocator_t *allocator;

    allocator = (wmem_simple_allocator_t *) private_data;

    wmem_free(NULL, ptr);
    allocator->count--;

    for (i = allocator->count; i >= 0; i--) {
        if (ptr == allocator->ptrs[i]) {
            if (i < allocator->count) {
                allocator->ptrs[i] = allocator->ptrs[allocator->count];
                allocator->sizes[i] = allocator->sizes[allocator->count];
            }
            return;
        }
    }

#if MEMORY_DEBUG
    D(("[wmem_simple_free :] ptr [0x%lx]\n", ptr));
#endif

    for (i = allocator->count; i >= 0; i--) {
        if (ptr == allocator->ptrs[i]) {
            if (i < allocator->count) {

#if MEMORY_DEBUG
                D(("[wmem_simple_free :] Freeing element %d\n", i));
#endif
                allocator->ptrs[i] = allocator->ptrs[allocator->count];
                allocator->sizes[i] = allocator->sizes[allocator->count];
            }

#if MEMORY_DEBUG
            dump_ptrs("simple_free", private_data);
#endif

            return;
        }
    }
#if MEMORY_DEBUG
    dump_ptrs("simple_free", private_data);
#endif
}

static void *
wmem_simple_realloc(void *private_data, void *ptr, const size_t size) {
    int i;
    wmem_simple_allocator_t *allocator;

    allocator = (wmem_simple_allocator_t *) private_data;

    for (i = allocator->count - 1; i >= 0; i--) {
        if (ptr == allocator->ptrs[i]) {
            if (size > allocator->sizes[i]) {
#if MEMORY_DEBUG
    D(("[wmem_simple_realloc :] ptr [0x%lx] size [0x%lx]\n", ptr, size));
#endif

    for (i = allocator->count - 1; i >= 0; i--) {
        if (ptr == allocator->ptrs[i]) {
            if (size > allocator->sizes[i]) {
#if MEMORY_DEBUG
                D(("[wmem_simple_realloc :] old ptr [0x%lx] old size [0x%lx]\n", allocator->ptrs[i], allocator->sizes[i]));
#endif

                // Grow
                void *new_ptr = (void **) wmem_alloc(NULL, size);
                memcpy(new_ptr, allocator->ptrs[i], allocator->sizes[i]);
                wmem_free(NULL, allocator->ptrs[i]);
                allocator->ptrs[i] = new_ptr;
                allocator->sizes[i] = size;
            }

#if MEMORY_DEBUG
                D(("[wmem_simple_realloc :] Grow : new ptr [0x%lx] new size [0x%lx]\n", allocator->ptrs[i], allocator->sizes[i]));
#endif
            }

#if MEMORY_DEBUG
            dump_ptrs("simple_realloc", private_data);
#endif
            return allocator->ptrs[i];
            // return allocator->ptrs[i] = wmem_realloc(NULL, allocator->ptrs[i], size);
        }
    }

#if MEMORY_DEBUG
    D(("[wmem_simple_realloc :] Failed to find pointer in array.\n"));
    dump_ptrs("simple_realloc", private_data);
#endif

    /* not reached */
    return NULL;
}

static void
wmem_simple_free_all(void *private_data) {
    wmem_simple_allocator_t *allocator;
    int i;

    allocator = (wmem_simple_allocator_t *) private_data;

    for (i = 0; i < allocator->count; i++) {
        wmem_free(NULL, allocator->ptrs[i]);
    }
    allocator->count = 0;
    allocator->size = 0;

#if MEMORY_DEBUG
    dump_ptrs("simple_free_all", private_data);
#endif
}

static void
wmem_simple_gc(void *private_data) {
    (void) (private_data);
    /* In this simple allocator, there is nothing to garbage-collect */
}

static void
wmem_simple_allocator_cleanup(void *private_data) {
    wmem_simple_allocator_t *allocator;

    allocator = (wmem_simple_allocator_t *) private_data;

    wmem_free(NULL, allocator->ptrs);
    wmem_free(NULL, allocator->sizes);
    wmem_free(NULL, allocator);
}

void
wmem_simple_allocator_init(wmem_allocator_t *allocator) {
    wmem_simple_allocator_t *simple_allocator;

    simple_allocator = wmem_new(NULL, wmem_simple_allocator_t);

    allocator->walloc = &wmem_simple_alloc;
    allocator->wrealloc = &wmem_simple_realloc;
    allocator->wfree = &wmem_simple_free;

    allocator->free_all = &wmem_simple_free_all;
    allocator->gc = &wmem_simple_gc;
    allocator->cleanup = &wmem_simple_allocator_cleanup;

    allocator->private_data = (void *) simple_allocator;

    simple_allocator->count = 0;
    simple_allocator->size = DEFAULT_ALLOCS;
    simple_allocator->ptrs = wmem_alloc_array(NULL, void*, DEFAULT_ALLOCS);
    simple_allocator->sizes = wmem_alloc_array(NULL, size_t, DEFAULT_ALLOCS);
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
