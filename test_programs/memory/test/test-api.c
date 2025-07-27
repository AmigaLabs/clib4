/* ----------------------------------------------------------------------------
Copyright (c) 2018-2020, Microsoft Research, Daan Leijen
This is free software; you can redistribute it and/or modify it under the
terms of the MIT license. A copy of the license can be found in the file
"LICENSE" at the root of this distribution.
-----------------------------------------------------------------------------*/
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Walloc-size-larger-than="
#endif

/*
Testing allocators is difficult as bugs may only surface after particular
allocation patterns. The main approach to testing _mimalloc_ is therefore
to have extensive internal invariant checking (see `page_is_valid` in `page.c`
for example), which is enabled in debug mode with `-DMI_DEBUG_FULL=ON`.
The main testing is then to run `mimalloc-bench` [1] using full invariant checking
to catch any potential problems over a wide range of intensive allocation bench
marks.

However, this does not test well for the entire API surface. In this test file
we therefore test the API over various inputs. Please add more tests :-)

[1] https://github.com/daanx/mimalloc-bench
*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
// #include <sys/types.h>
#include "testhelper.h"
// #include "mitypes.h"
#define MI_SMALL_SIZE_MAX   (128*sizeof(void*))

#if !defined(MI_DEBUG_UNINIT)
#define MI_DEBUG_UNINIT     (0xD0)
#endif
#if !defined(MI_DEBUG_FREED)
#define MI_DEBUG_FREED      (0xDF)
#endif
#if !defined(MI_DEBUG_PADDING)
#define MI_DEBUG_PADDING    (0xDE)
#endif


// ---------------------------------------------------------------------------
// Test functions
// ---------------------------------------------------------------------------
bool test_heap1(void);
bool test_heap2(void);
bool test_stl_allocator1(void);
bool test_stl_allocator2(void);

bool test_stl_heap_allocator1(void);
bool test_stl_heap_allocator2(void);
bool test_stl_heap_allocator3(void);
bool test_stl_heap_allocator4(void);

bool mem_is_zero(uint8_t* p, size_t size) {
  if (p==NULL) return false;
  for (size_t i = 0; i < size; ++i) {
    if (p[i] != 0) return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// Main testing
// ---------------------------------------------------------------------------
int main(void) {  

  // ---------------------------------------------------
  // Malloc
  // ---------------------------------------------------

  CHECK_BODY("malloc-zero") {
    void* p = malloc(0);
    result = (p != NULL);
    free(p);
  };
  CHECK_BODY("malloc-nomem1") {
    result = (malloc((size_t)PTRDIFF_MAX + (size_t)1) == NULL);
  };
  CHECK_BODY("malloc-null") {
    free(NULL);
  };
  CHECK_BODY("calloc-overflow") {
    // use (size_t)&mi_calloc to get some number without triggering compiler warnings
    result = (calloc((size_t)&calloc,SIZE_MAX/1000) == NULL);
  };
  CHECK_BODY("malloc-large") {   // see PR #544.
    void* p = malloc(67108872);
    free(p);
  };

  // ---------------------------------------------------
  // Extended
  // ---------------------------------------------------
  CHECK_BODY("posix_memalign1") {
    void* p = &p;
    int err = posix_memalign(&p, sizeof(void*), 32);
    result = ((err==0 && (uintptr_t)p % sizeof(void*) == 0) || p==&p);
    free(p);
  };
  CHECK_BODY("posix_memalign_no_align") {
    void* p = &p;
    int err = posix_memalign(&p, 3, 32);
    result = (err==EINVAL && p==&p);
  };
  CHECK_BODY("posix_memalign_zero") {
    void* p = &p;
    int err = posix_memalign(&p, sizeof(void*), 0);
    free(p);
    result = (err==0);
  };
  CHECK_BODY("posix_memalign_nopow2") {
    void* p = &p;
    int err = posix_memalign(&p, 3*sizeof(void*), 32);
    result = (err==EINVAL && p==&p);
  };
  CHECK_BODY("posix_memalign_nomem") {
    void* p = &p;
    int err = posix_memalign(&p, sizeof(void*), SIZE_MAX);
    result = (err==ENOMEM && p==&p);
  };

  // ---------------------------------------------------
  // Aligned API
  // ---------------------------------------------------
  CHECK_BODY("memalign1") {
    void* p;
    bool ok = true;
    for (int i = 0; i < 8 && ok; i++) {
      p = memalign(16,8);
      ok = (p != NULL && (uintptr_t)(p) % 16 == 0); free(p);
    }
    result = ok;
  };

  // ---------------------------------------------------
  // Reallocation
  // ---------------------------------------------------
  CHECK_BODY("realloc-null") {
    void* p = realloc(NULL,4);
    result = (p != NULL);
    free(p);
  };

  CHECK_BODY("realloc-null-sizezero") {
    void* p = realloc(NULL,0);  // <https://en.cppreference.com/w/c/memory/realloc> "If ptr is NULL, the behavior is the same as calling malloc(new_size)."
    result = (p != NULL);
    free(p);
  };

  CHECK_BODY("realloc-sizezero") {
    void* p = malloc(4);
    void* q = realloc(p, 0);
    result = (q != NULL);
    free(q);
  };

  // ---------------------------------------------------
  // Heaps
  // ---------------------------------------------------
  // ---------------------------------------------------
  // Done
  // ---------------------------------------------------[]
  return print_test_summary();
}

// ---------------------------------------------------
// Larger test functions
// ---------------------------------------------------
