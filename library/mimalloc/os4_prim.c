/* ----------------------------------------------------------------------------
Copyright (c) 2018-2025, Microsoft Research, Daan Leijen, Alon Zakai
This is free software; you can redistribute it and/or modify it under the
terms of the MIT license. A copy of the license can be found in the file
"LICENSE" at the root of this distribution.
-----------------------------------------------------------------------------*/

// This file is included in `src/prim/prim.c`

#include "mimalloc.h"
#include "mimalloc/internal.h"
#include "mimalloc/atomic.h"
#include "mimalloc/prim.h"

#include <proto/exec.h>
#include <proto/dos.h>

#include <stdio.h>         // fputs
#include <unistd.h>        // sysconf, getpagesize
#include <fcntl.h>         // open, close, read, access
#include <stdlib.h>        // getenv, arc4random_buf
#include <sys/time.h>      // getrusage
#include <sys/resource.h>  // getrusage

// Design
// ======
//
// mimalloc is built on top of os4malloc. os4malloc is a minimal allocator on top
// of AllocVecTags. The reason for having three layers here is that we want mimalloc to
// be able to allocate and release system memory properly, the same way it would
// when using VirtualAlloc on Windows or mmap on POSIX, and sbrk is missing.
// Specifically, sbrk can only go up and down, and not "skip" over regions, and
// so we end up either never freeing memory to the system, or we can get stuck
// with holes.
//
// Even without that issue, we want our system allocator to handle holes, that
// is, it should merge freed regions and allow allocating new content there of
// the full size, etc., so that we do not waste space. That means that the
// system allocator really does need to handle the general problem of allocating
// and freeing variable-sized chunks of memory in a random order, like malloc/
// free do. And so it makes sense to layer mimalloc on top of such an
// implementation.
//
// os4malloc makes sense for the lower level because it is small and simple while
// still fully handling merging of holes etc. It is not the most efficient
// allocator, but our assumption is that mimalloc needs to be fast while the
// system allocator underneath it is called much less frequently.
//

//---------------------------------------------
// init
//---------------------------------------------
#define MIN_OS4MALLOC_ALIGN           16

void _mi_prim_mem_init(mi_os_mem_config_t *config) {
	config->page_size = sysconf(_SC_PAGESIZE);
	config->alloc_granularity = 16;
	config->has_overcommit = false;
	config->has_partial_free = false;
	config->has_virtual_reserve = false;
}

int _mi_prim_free(void *addr, size_t size) {
	if (size == 0)
		return 0;
	if (addr)
		FreeVec(addr);
	return 0;
}

//---------------------------------------------
// Allocation
//---------------------------------------------

// Note: the `try_alignment` is just a hint and the returned pointer is not guaranteed to be aligned.
int _mi_prim_alloc(void *hint_addr, size_t size, size_t try_alignment, bool commit, bool allow_large, bool *is_large, bool *is_zero, void **addr) {
	MI_UNUSED(try_alignment);
	MI_UNUSED(allow_large);
	MI_UNUSED(commit);
	MI_UNUSED(hint_addr);
	*is_large = false;
	// TODO: Track the highest address ever seen; first uses of it are zeroes.
	//       That assumes no one else uses sbrk but us (they could go up,
	//       scribble, and then down), but we could assert on that perhaps.
	*is_zero = false;
	// os4malloc has a minimum alignment size.
	if (try_alignment < MIN_OS4MALLOC_ALIGN) {
		try_alignment = MIN_OS4MALLOC_ALIGN;
	}
	void *p = AllocVecTags(size, AVT_Type, MEMF_SHARED, AVT_Alignment, try_alignment, TAG_DONE);
	*addr = p;
	if (p == 0) {
		return ENOMEM;
	}
	return 0;
}

//---------------------------------------------
// Commit/Reset
//---------------------------------------------

int _mi_prim_commit(void *addr, size_t size, bool *is_zero) {
	MI_UNUSED(addr);
	MI_UNUSED(size);
	// See TODO above.
	*is_zero = false;
	return 0;
}

int _mi_prim_decommit(void *addr, size_t size, bool *needs_recommit) {
	MI_UNUSED(addr);
	MI_UNUSED(size);
	*needs_recommit = false;
	return 0;
}

int _mi_prim_reset(void *addr, size_t size) {
	MI_UNUSED(addr);
	MI_UNUSED(size);
	return 0;
}

int _mi_prim_reuse(void *addr, size_t size) {
	MI_UNUSED(addr);
	MI_UNUSED(size);
	return 0;
}

int _mi_prim_protect(void *addr, size_t size, bool protect) {
	MI_UNUSED(addr);
	MI_UNUSED(size);
	MI_UNUSED(protect);
	return 0;
}

//---------------------------------------------
// Huge pages and NUMA nodes
//---------------------------------------------

int _mi_prim_alloc_huge_os_pages(void *hint_addr, size_t size, int numa_node, bool *is_zero, void **addr) {
	MI_UNUSED(hint_addr);
	MI_UNUSED(size);
	MI_UNUSED(numa_node);
	*is_zero = true;
	*addr = NULL;
	return ENOSYS;
}

size_t _mi_prim_numa_node(void) {
	return 0;
}

size_t _mi_prim_numa_node_count(void) {
	return 1;
}

//----------------------------------------------------------------
// Clock
//----------------------------------------------------------------

mi_msecs_t _mi_prim_clock_now(void) {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return ((mi_msecs_t) t.tv_sec * 1000) + ((mi_msecs_t) t.tv_nsec / 1000000);
}

//----------------------------------------------------------------
// Process info
//----------------------------------------------------------------

static mi_msecs_t timeval_secs(const struct timeval *tv) {
	return ((mi_msecs_t) tv->tv_sec * 1000L) + ((mi_msecs_t) tv->tv_usec / 1000L);
}

void _mi_prim_process_info(mi_process_info_t *pinfo) {
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	pinfo->utime = timeval_secs(&rusage.ru_utime);
	pinfo->stime = timeval_secs(&rusage.ru_stime);
	pinfo->peak_rss = rusage.ru_maxrss * 1024;
}

//----------------------------------------------------------------
// Output
//----------------------------------------------------------------

void _mi_prim_out_stderr(const char *msg) {
	Printf("%s\n", msg);
}

//----------------------------------------------------------------
// Environment
//----------------------------------------------------------------

static char *my_strcpy(char *dest, const char *src) {
	char *original_dest = dest; // Store the original destination pointer to return it

	// Loop through the source string until the null terminator is found
	while (*src != '\0') {
		*dest = *src; // Copy the character
		dest++;       // Move to the next position in destination
		src++;        // Move to the next position in source
	}

	*dest = '\0'; // Copy the null terminator

	return original_dest; // Return the original destination pointer
}

bool _mi_prim_getenv(const char *name, char *result, size_t result_size) {
	static char env_var_buffer[FILENAME_MAX] = {0};

	if (name == NULL) {
		return false;
	}

	if (GetVar((STRPTR) name, env_var_buffer, FILENAME_MAX, 0) < 0) {
		return false;
	}
	my_strcpy(result, env_var_buffer);
	return true;
}

//----------------------------------------------------------------
// Random
//----------------------------------------------------------------

bool _mi_prim_random_buf(void *buf, size_t buf_len) {
	// TODO: This will slow down everything. Use something else doesn't use __clib4 internally
Printf("arc4random_buf\n");
	arc4random_buf(buf, buf_len);
	return true;
}

void _mi_prim_thread_init_auto_done(void) {
	// nothing
}

void _mi_prim_thread_done_auto_done(void) {
	// nothing
}

void _mi_prim_thread_associate_default_heap(mi_heap_t *heap) {
	MI_UNUSED(heap);
}

int32_t __popcountsi2(int32_t a) ;

int32_t __popcountsi2(int32_t a) {
	uint32_t x = (uint32_t)a;
	x = x - ((x >> 1) & 0x55555555);
	// Every 2 bits holds the sum of every pair of bits
	x = ((x >> 2) & 0x33333333) + (x & 0x33333333);
	// Every 4 bits holds the sum of every 4-set of bits (3 significant bits)
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	// Every 8 bits holds the sum of every 8-set of bits (4 significant bits)
	x = (x + (x >> 16));
	// The lower 16 bits hold two 8 bit sums (5 significant bits).
	//    Upper 16 bits are garbage
	return (x + (x >> 8)) & 0x0000003F; // (6 significant bits)
}
