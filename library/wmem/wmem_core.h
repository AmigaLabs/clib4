/** @file
 * Definitions for the Wireshark Memory Manager Core
 * Copyright 2012, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __WMEM_CORE_H__
#define __WMEM_CORE_H__

#include <features.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

__BEGIN_DECLS

/** @defgroup wmem Wireshark Memory Manager
 *
 * Wmem is a memory management framework for Wireshark that makes it simple to
 * write dissectors (and other 'user-space' code) that doesn't leak memory. The
 * core module provides basic functions like malloc, realloc and free, but
 * many other functions are available (see the "Modules" list at the top of
 * the generated doxygen HTML).
 *
 * Any wmem functions which allocate memory are guaranteed to either succeed or
 * abort the program. However, they *can* still legally return NULL when the
 * amount of requested memory is zero.
 *
 * @{
 */

// Set to 1 if you want to track memory allocations
#ifndef MEMORY_DEBUG
#define MEMORY_DEBUG 0
#else
#ifndef DEBUG
#error "MEMORY_DEBUG NEEDS DEBUG FLAG ON"
#endif
#endif

struct _wmem_allocator_t;
/** A public opaque type representing one wmem allocation pool. */
typedef struct _wmem_allocator_t wmem_allocator_t;

/** An enumeration of the different types of available allocators. */
typedef enum _wmem_allocator_type_t {
    WMEM_ALLOCATOR_SIMPLE, /**< A trivial allocator that mallocs requested
                memory and tracks allocations via a hash table. As simple as
                possible, intended more as a demo than for practical usage. Also
                has the benefit of being friendly to tools like valgrind. */
    WMEM_ALLOCATOR_BLOCK, /**< A block allocator that grabs large chunks of
                memory at a time (8 MB currently) and serves allocations out of
                those chunks. Designed for efficiency, especially in the
                free_all operation. */
    WMEM_ALLOCATOR_STRICT, /**< An allocator that does its best to find invalid
                memory usage via things like canaries and scrubbing freed
                memory. Valgrind is the better choice on platforms that support
                it. */
    WMEM_ALLOCATOR_BLOCK_FAST /**< A block allocator like WMEM_ALLOCATOR_BLOCK
                but even faster by tracking absolutely minimal metadata and
                making 'free' a no-op. Useful only for very short-lived scopes
                where there's no reason to free individual allocations because
                the next free_all is always just around the corner. */
} wmem_allocator_type_t;

/** Allocate the requested amount of memory in the given pool.
 *
 * @param allocator The allocator object to use to allocate the memory.
 * @param size The amount of memory to allocate.
 * @return A void pointer to the newly allocated memory.
 */
extern void * wmem_alloc(wmem_allocator_t *allocator, const size_t size) __attribute__((__malloc__)) __attribute__((__alloc_size__(2)));

/** Allocate memory sufficient to hold one object of the given type.
 *
 * @param allocator The allocator object to use to allocate the memory.
 * @param type The type that the newly allocated memory will hold.
 * @return A void pointer to the newly allocated memory.
 */
#define wmem_new(allocator, type) \
    ((type*)wmem_alloc((allocator), sizeof(type)))

/*
 * Overflow-safe multiplication of the size of a type by a number of
 * items of that type, returning 0 if the result would overflow (or
 * if the number of elements is negative), and the product otherwise.
 */
#define wmem_safe_mult_type_size(type, num) \
    ((((num) <= 0) || ((size_t)sizeof(type) > (INT_MAX / (size_t)(num)))) ? 0 : (sizeof(type) * (num)))

/** Allocate memory sufficient to hold n objects of the given type.
 *
 * @param allocator The allocator object to use to allocate the memory.
 * @param type The type that the newly allocated memory will hold.
 * @param num  The number of objects that the newly allocated memory will hold.
 * @return A void pointer to the newly allocated memory.
 */
#define wmem_alloc_array(allocator, type, num) \
    ((type*)wmem_alloc((allocator), wmem_safe_mult_type_size(type, (num))))

/** Allocate the requested amount of memory in the given pool. Initializes the
 * allocated memory with zeroes.
 *
 * @param allocator The allocator object to use to allocate the memory.
 * @param size The amount of memory to allocate.
 * @return A void pointer to the newly allocated and zeroed memory.
 */
extern void * wmem_alloc0(wmem_allocator_t *allocator, const size_t size) __attribute__((__malloc__)) __attribute__((__alloc_size__(2)));

/** Allocate memory sufficient to hold one object of the given type.
 * Initializes the allocated memory with zeroes.
 *
 * @param allocator The allocator object to use to allocate the memory.
 * @param type The type that the newly allocated memory will hold.
 * @return A void pointer to the newly allocated and zeroed memory.
 */
#define wmem_new0(allocator, type) \
    ((type*)wmem_alloc0((allocator), sizeof(type)))

/** Allocate memory sufficient to hold n objects of the given type.
 * Initializes the allocated memory with zeroes.
 *
 * @param allocator The allocator object to use to allocate the memory.
 * @param type The type that the newly allocated memory will hold.
 * @param num  The number of objects that the newly allocated memory will hold.
 * @return A void pointer to the newly allocated and zeroed memory.
 */
#define wmem_alloc0_array(allocator, type, num) \
    ((type*)wmem_alloc0((allocator), wmem_safe_mult_type_size(type, (num))))

/** Returns the allocated memory to the allocator. This function should only
 * be called directly by allocators when the allocated block is sufficiently
 * large that the reduced memory usage is worth the cost of the extra function
 * call. It's usually easier to just let it get cleaned up when wmem_free_all()
 * is called.
 *
 * @param allocator The allocator object used to originally allocate the memory.
 * @param ptr The pointer to the memory block to free. After this function
 * returns it no longer points to valid memory.
 */
extern void wmem_free(wmem_allocator_t *allocator, void *ptr);

/** Resizes a block of memory, potentially moving it if resizing it in place
 * is not possible.
 *
 * @param allocator The allocator object used to originally allocate the memory.
 * @param ptr The pointer to the memory block to resize.
 * @param size The new size for the memory block.
 * @return The new location of the memory block. If this is different from ptr
 * then ptr no longer points to valid memory.
 */
extern void * wmem_realloc(wmem_allocator_t *allocator, void *ptr, const size_t size) __attribute__((__alloc_size__(3)));

/** Frees all the memory allocated in a pool. Depending on the allocator
 * implementation used this can be significantly cheaper than calling
 * wmem_free() on all the individual blocks. It also doesn't require you to have
 * external pointers to those blocks.
 *
 * @param allocator The allocator to free the memory from.
 */
extern void wmem_free_all(wmem_allocator_t *allocator);

/** Triggers a garbage-collection in the allocator. This does not free any
 * memory, but it can return unused blocks to the operating system or perform
 * other optimizations.
 *
 * @param allocator The allocator in which to trigger the garbage collection.
 */
extern void wmem_gc(wmem_allocator_t *allocator);

/** Destroy the given allocator, freeing all memory allocated in it. Once this
 * function has been called, no memory allocated with the allocator is valid.
 *
 * @param allocator The allocator to destroy.
 */
extern void wmem_destroy_allocator(wmem_allocator_t *allocator);

/** Create a new allocator of the given type. The type may be overridden by the
 * WIRESHARK_DEBUG_WMEM_OVERRIDE environment variable.
 *
 * @param type The type of allocator to create.
 * @return The new allocator.
 */
extern wmem_allocator_t * wmem_allocator_new(const wmem_allocator_type_t type);

/** Initialize the wmem subsystem. This must be called before any other wmem
 * function, usually at the very beginning of your program.
 */
extern void wmem_init(void);

/** Teardown the wmem subsystem. This must be called after all other wmem
 * functions, usually at the very end of your program. This function will not
 * destroy outstanding allocators, you must do that yourself.
 */
extern void wmem_cleanup(void);
extern void wmem_enter_scope(wmem_allocator_t *allocator);
extern void wmem_leave_scope(wmem_allocator_t *allocator);
extern bool wmem_in_scope(wmem_allocator_t *allocator);

typedef int32_t (*GCompareFunc) (const void *a, const void *b);
typedef void (*GFunc) (void *data, void *user_data);
typedef bool (*GHRFunc)  (void *key, void *value, void *user_data);
typedef uint32_t (*GHashFunc) (const void *key);
typedef bool (*GEqualFunc) (const void *a, const void *b);
typedef void (*GHFunc) (void *key, void *value, void *user_data);

#define GPOINTER_TO_INT(p) ((int) (int) (p))
#define GPOINTER_TO_UINT(p)	((unsigned int) (unsigned int) (p))
#define GUINT_TO_POINTER(u)	((void *) (unsigned int) (u))

/** @} */

__END_DECLS

#endif /* __WMEM_CORE_H__ */
