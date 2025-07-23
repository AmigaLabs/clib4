#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
// Helper functions
// ---------------------------------------------------------------------------
bool check_zero_init(uint8_t* p, size_t size);
bool check_debug_fill_uninit(uint8_t* p, size_t size);
bool check_debug_fill_freed(uint8_t* p, size_t size);

// ---------------------------------------------------------------------------
// Main testing
// ---------------------------------------------------------------------------
int main(void) {

  CHECK_BODY("zeroinit-calloc-small") {
    size_t calloc_size = MI_SMALL_SIZE_MAX / 2;
    uint8_t* p = (uint8_t*)calloc(calloc_size, 1);
    result = check_zero_init(p, calloc_size);
    free(p);
  };
  CHECK_BODY("zeroinit-calloc-large") {
    size_t calloc_size = MI_SMALL_SIZE_MAX * 2;
    uint8_t* p = (uint8_t*)calloc(calloc_size, 1);
    result = check_zero_init(p, calloc_size);
    free(p);
  };

  // ---------------------------------------------------
  // Debug filling
  // ---------------------------------------------------
  CHECK_BODY("uninit-malloc-small") {
    size_t malloc_size = MI_SMALL_SIZE_MAX / 2;
    uint8_t* p = (uint8_t*)malloc(malloc_size);
    result = check_debug_fill_uninit(p, malloc_size);
    free(p);
  };
  CHECK_BODY("uninit-malloc-large") {
    size_t malloc_size = MI_SMALL_SIZE_MAX * 2;
    uint8_t* p = (uint8_t*)malloc(malloc_size);
    result = check_debug_fill_uninit(p, malloc_size);
    free(p);
  };

  CHECK_BODY("uninit-realloc-small") {
    size_t malloc_size = MI_SMALL_SIZE_MAX / 2;
    uint8_t* p = (uint8_t*)malloc(malloc_size);
    result = check_debug_fill_uninit(p, malloc_size);
    malloc_size *= 3;
    p = (uint8_t*)realloc(p, malloc_size);
    result &= check_debug_fill_uninit(p, malloc_size);
    free(p);
  };
  CHECK_BODY("uninit-realloc-large") {
    size_t malloc_size = MI_SMALL_SIZE_MAX * 2;
    uint8_t* p = (uint8_t*)malloc(malloc_size);
    result = check_debug_fill_uninit(p, malloc_size);
    malloc_size *= 3;
    p = (uint8_t*)realloc(p, malloc_size);
    result &= check_debug_fill_uninit(p, malloc_size);
    free(p);
  };

  CHECK_BODY("fill-freed-small") {
    size_t malloc_size = MI_SMALL_SIZE_MAX / 2;
    uint8_t* p = (uint8_t*)malloc(malloc_size);
    free(p);
    // First sizeof(void*) bytes will contain housekeeping data, skip these
    result = check_debug_fill_freed(p + sizeof(void*), malloc_size - sizeof(void*));
  };
  CHECK_BODY("fill-freed-large") {
    size_t malloc_size = MI_SMALL_SIZE_MAX * 2;
    uint8_t* p = (uint8_t*)malloc(malloc_size);
    free(p);
    // First sizeof(void*) bytes will contain housekeeping data, skip these
    result = check_debug_fill_freed(p + sizeof(void*), malloc_size - sizeof(void*));
  };

  // ---------------------------------------------------
  // Done
  // ---------------------------------------------------[]
  return print_test_summary();
}

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------
bool check_zero_init(uint8_t* p, size_t size) {
  if(!p)
    return false;
  bool result = true;
  for (size_t i = 0; i < size; ++i) {
    result &= p[i] == 0;
  }
  return result;
}

bool check_debug_fill_uninit(uint8_t* p, size_t size) {
#if MI_TRACK_VALGRIND || MI_TRACK_ASAN
  (void)p; (void)size;
  return true; // when compiled with valgrind we don't init on purpose
#else
  if(!p)
    return false;

  bool result = true;
  for (size_t i = 0; i < size; ++i) {
    result &= p[i] == MI_DEBUG_UNINIT;
  }
  return result;
#endif
}

bool check_debug_fill_freed(uint8_t* p, size_t size) {
#if MI_TRACK_VALGRIND
  (void)p; (void)size;
  return true; // when compiled with valgrind we don't fill on purpose
#else
  if(!p)
    return false;

  bool result = true;
  for (size_t i = 0; i < size; ++i) {
    result &= p[i] == MI_DEBUG_FREED;
  }
  return result;
#endif
}