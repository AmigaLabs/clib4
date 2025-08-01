/* wmem_core.c
 * Wireshark Memory Manager Core
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

#include "wmem-int.h"
#include "wmem_core.h"
#include "wmem_map_int.h"
#include "wmem_user_cb_int.h"
#include "wmem_allocator.h"
#include "wmem_allocator_simple.h"
#include "wmem_allocator_block.h"
#include "wmem_allocator_block_fast.h"
#include "wmem_allocator_strict.h"

// #define ULTRA_MEMORY_DEBUG
#ifdef ULTRA_MEMORY_DEBUG
typedef struct mem_entry {
    void *addr;
    size_t size;
    bool freed;
    struct mem_entry *next;
} mem_entry;
static mem_entry *m_list = 0;
void insert_mem_entry(void *addr, size_t size);
void insert_mem_entry(void *addr, size_t size) {
    mem_entry *new = AllocVecTags(sizeof(mem_entry), TAG_DONE);
    new->addr = addr;
    new->size = size;
    new->freed = false;
    new->next = m_list;
    m_list = new;
}
void free_mem_entry(void *addr, size_t size);
void free_mem_entry(void *addr, size_t size) {
    for(mem_entry *e = m_list; e; e = e->next) {
        if(e->addr == addr) {
            e->freed = true;
        }
    }
}
bool check_mem_list();
bool check_mem_list() {
    bool success = true;
    for(mem_entry *e = m_list; e; e = e->next) {
        if(e->freed == false) {
            // DebugPrintF("[wmem :] ++++ CATASTROPHY : Unfreed block: ptr = 0x%lx, size = %ld. ++++\n", e->addr, e->size);
            success = false;
        }
    }
    return success;
}
void free_mem_list();
void free_mem_list() {
    for(mem_entry *e = m_list; e;) {
        mem_entry *next = e->next;
        FreeVec(e);
        e = next;
    }
    m_list = 0;
}
#endif
/* Set according to the WIRESHARK_DEBUG_WMEM_OVERRIDE environment variable in
 * wmem_init. Should not be set again. */
static bool do_override;
static wmem_allocator_type_t override_type;

static inline bool
power_of_two(size_t alignment) {
    return (alignment != 0) && ((alignment & (alignment - 1)) == 0);
}

uintptr_t align_address(uintptr_t address, size_t alignment) {
    return (address + alignment - 1) & ~(alignment - 1);
}

#ifdef MEMORY_DEBUG
static int allocs = 0;
#endif

void *
wmem_alloc_aligned(wmem_allocator_t *allocator, const size_t size, int32_t alignment) {

#if MEMORY_DEBUG
    D(("Allocating %ld bytes chunk of memory (alignment: %ld).\n", size, alignment));
#endif

    if (allocator == NULL) {

#ifdef MEMORY_DEBUG
        if(!power_of_two(alignment)) {
            D(("[wmem_alloc :] <FAULT> AllocVecTags can only align to powers of two.\n"));
        }
        D(("[wmem_alloc :] Allocating system chunk, size : 0x%lx, alignment : %ld", size, alignment));
#endif

        void *r = AllocVecTags(size, AVT_Type, MEMF_SHARED, AVT_Alignment, alignment, TAG_DONE);

#ifdef ULTRA_MEMORY_DEBUG
        if (r) {
            insert_mem_entry(r, size);
        }
#endif

        if (!r) errno = ENOMEM;
#ifdef MEMORY_DEBUG
        else allocs++;
#endif

        return r;
    }

    assert(allocator->in_scope);

    if (size == 0) {
        return NULL;
    }

    void *result = allocator->walloc(allocator->private_data, size, alignment);
    return result;
}
void *
wmem_alloc(wmem_allocator_t *allocator, const size_t size) {
    return wmem_alloc_aligned(allocator, size, 16);
}

void *
wmem_alloc0(wmem_allocator_t *allocator, const size_t size) {
    void *buf;

    buf = wmem_alloc(allocator, size);

    if (buf) {
        memset(buf, 0, size);
    }

    return buf;
}

void
wmem_free(wmem_allocator_t *allocator, void *ptr) {
    if (allocator == NULL) {

        FreeVec(ptr);

#ifdef ULTRA_MEMORY_DEBUG
        free_mem_entry(ptr, -1);
#endif

#ifdef MEMORY_DEBUG
        allocs--;
#endif

        ptr = NULL;
        return;
    }

    assert(allocator->in_scope);

    if (ptr == NULL) {
        return;
    }

    // wmem_block_verify(allocator);

    allocator->wfree(allocator->private_data, ptr);
}

void *
wmem_realloc_aligned(wmem_allocator_t *allocator, void *ptr, const size_t size, int32_t alignment) {
    if (allocator == NULL) {
        // Since we have no generic way of determining the old size,
        //  this feature cannot be supported on amigaos.
        // It needs to be emulated in the specific allocators.
        SHOWMSG("unsupported feature (realloc). now we are going to fail\n");
        return NULL;
    }

    if (ptr == NULL) {
        void *result = wmem_alloc_aligned(allocator, size, alignment);
        return result;
    }

    if (size == 0) {
        wmem_free(allocator, ptr);
        return NULL;
    }

    assert(allocator->in_scope);

    void *result = allocator->wrealloc(allocator->private_data, ptr, size, alignment);
    return result;
}
void *
wmem_realloc(wmem_allocator_t *allocator, void *ptr, const size_t size) {
    return wmem_realloc_aligned(allocator, ptr, size, 16);
}

static void
wmem_free_all_real(wmem_allocator_t *allocator, bool final) {
    wmem_call_callbacks(allocator,
                        final ? WMEM_CB_DESTROY_EVENT : WMEM_CB_FREE_EVENT);
    allocator->free_all(allocator->private_data);
}

void
wmem_free_all(wmem_allocator_t *allocator) {
    wmem_free_all_real(allocator, false);
}

void
wmem_gc(wmem_allocator_t *allocator) {
    allocator->gc(allocator->private_data);
}

void
wmem_destroy_allocator(wmem_allocator_t *allocator) {

    wmem_free_all_real(allocator, true);
    allocator->cleanup(allocator->private_data);
#ifdef ULTRA_MEMORY_DEBUG
    if(check_mem_list()) {
        DebugPrintF("[wmem :] ++++ SUCCESS : All memory freed from allocator. ++++\n");
    } else {
        DebugPrintF("[wmem :] ++++ End of Allocator scope : NOT all memory freed from allocator. ++++\n");
    }
    FreeVec(allocator);
#else
    wmem_free(NULL, allocator);
#endif

#ifdef MEMORY_DEBUG
    D(("[END OF SESSION] +++++ Allocs: %ld ++++++\n", allocs));
#endif
}

wmem_allocator_t *
wmem_allocator_new(const wmem_allocator_type_t type) {
    wmem_allocator_t *allocator;
    wmem_allocator_type_t real_type;

#ifdef MEMORY_DEBUG
    D(("[new :] Create new allocator ++++ ***** ++++\n"));
#endif

    if (do_override) {
        real_type = override_type;
    } else {
        real_type = type;
    }

    allocator =
#ifdef ULTRA_MEMORY_DEBUG
    AllocVecTags(sizeof(wmem_allocator_t), TAG_DONE);
#else
    wmem_new(NULL, wmem_allocator_t);
#endif
    allocator->type = real_type;
    allocator->callbacks = NULL;
    allocator->in_scope = true;

    switch (real_type) {
        case WMEM_ALLOCATOR_SIMPLE:
            wmem_simple_allocator_init(allocator);
            break;
        case WMEM_ALLOCATOR_BLOCK:
            wmem_block_allocator_init(allocator);
            break;
        case WMEM_ALLOCATOR_BLOCK_FAST:
            wmem_block_fast_allocator_init(allocator);
            break;
        case WMEM_ALLOCATOR_STRICT:
            wmem_strict_allocator_init(allocator);
            break;
        default:
            break;
    };

    return allocator;
}

void
wmem_init(void) {
    const char *override_env;

    /* Our valgrind script uses this environment variable to override the
     * usual allocator choice so that everything goes through system-level
     * allocations that it understands and can track. Otherwise it will get
     * confused by the block allocator etc. */
    override_env = getenv("WIRESHARK_DEBUG_WMEM_OVERRIDE");

    if (override_env == NULL) {
        do_override = false;
    } else {
        do_override = true;
        if (strncmp(override_env, "simple", strlen("simple")) == 0) {
            override_type = WMEM_ALLOCATOR_SIMPLE;
        } else if (strncmp(override_env, "block", strlen("block")) == 0) {
            override_type = WMEM_ALLOCATOR_BLOCK;
        } else if (strncmp(override_env, "strict", strlen("strict")) == 0) {
            override_type = WMEM_ALLOCATOR_STRICT;
        } else if (strncmp(override_env, "block_fast", strlen("block_fast")) == 0) {
            override_type = WMEM_ALLOCATOR_BLOCK_FAST;
        } else {
            SHOWMSG("Unrecognized wmem override");
            do_override = false;
        }
    }

    wmem_init_hashing();
}

void
wmem_cleanup(void) {
}

void
wmem_enter_scope(wmem_allocator_t *allocator) {
    allocator->in_scope = true;
}

void
wmem_leave_scope(wmem_allocator_t *allocator) {
    wmem_free_all(allocator);
    allocator->in_scope = false;
}

bool
wmem_in_scope(wmem_allocator_t *allocator) {
    return allocator->in_scope;
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
