//===---------- emutls.c - Implements __emutls_get_address ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <proto/exec.h>

typedef unsigned int pthread_key_t;

typedef struct {
    void (*destructor)(void *);
    BOOL used;
} TLSKey;

typedef struct {
    struct Process *parent;
    struct Process *task;
    void *tlsvalues[64];
    struct MinList cleanup;
} ThreadInfo;

typedef struct emutls_address_array {
    uintptr_t skip_destructor_rounds;
    uintptr_t size; // number of elements in the 'data' array
    void *data[];
} emutls_address_array;

static const TLSKey tlskeys[64];
static const ThreadInfo threads[1];

static void emutls_shutdown(emutls_address_array *array);

static APTR emutls_mutex;
static pthread_key_t emutls_pthread_key;
static bool emutls_key_created = false;

static inline void set_tls_register(ThreadInfo *ti) {
    __asm__ volatile("mr r2, %0" :: "r"(ti));
}

static ThreadInfo *
_GetCurrentThreadInfo() {
    ThreadInfo *ti;
    __asm__("mr %0, r2" : "=r"(ti));
    return ti;
}

int
_pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
    TLSKey *tls;
    int i;

    if (key == NULL)
        return EINVAL;

    //ObtainSemaphore(&tls_sem);

    for (i = 0; i < 64; i++) {
        if (tlskeys[i].used == FALSE)
            break;
    }

    if (i == 64) {
        //ReleaseSemaphore(&tls_sem);
        return EAGAIN;
    }

    tls = &tlskeys[i];
    tls->used = TRUE;
    tls->destructor = destructor;

    //ReleaseSemaphore(&tls_sem);

    *key = i;

    return 0;
}

void *
_pthread_getspecific(pthread_key_t key) {
    ThreadInfo *inf;
    void *value = NULL;

    if (key >= PTHREAD_KEYS_MAX || key < 0)
        return NULL;

    inf = _GetCurrentThreadInfo();
    if (inf != NULL)
        value = inf->tlsvalues[key];

    return value;
}

int
_pthread_setspecific(pthread_key_t key, const void *value) {
    ThreadInfo *inf;

    if (key >= 64 || key < 0)
        return EINVAL;

    inf = _GetCurrentThreadInfo();
    if (inf != NULL)
        inf->tlsvalues[key] = (void *) value;

    return 0;
}

typedef unsigned int gcc_word __attribute__((mode(word)));
typedef unsigned int gcc_pointer __attribute__((mode(pointer)));

static __inline void *emutls_memalign_alloc(size_t align, size_t size) {
#define EXTRA_ALIGN_PTR_BYTES (align - 1 + sizeof(void *))
    void *base = AllocVecTags(EXTRA_ALIGN_PTR_BYTES + size, AVT_Type, MEMF_SHARED, AVT_Alignment, align, TAG_DONE);
    if (base == NULL)
        abort();
    return base;
}

static __inline void emutls_memalign_free(void *base) {
    if (base)
        FreeVec(base);
}

static __inline void emutls_setspecific(emutls_address_array *value) {
    _pthread_setspecific(emutls_pthread_key, (void *) value);
}

static __inline emutls_address_array *emutls_getspecific(void) {
    return (emutls_address_array *) _pthread_getspecific(emutls_pthread_key);
}

static void emutls_key_destructor(void *ptr) {
    emutls_address_array *array = (emutls_address_array *) ptr;
    if (array->skip_destructor_rounds > 0) {
        // emutls is deallocated using a pthread key destructor. These
        // destructors are called in several rounds to accommodate destructor
        // functions that (re)initialize key values with pthread_setspecific.
        // Delay the emutls deallocation to accommodate other end-of-thread
        // cleanup tasks like calling thread_local destructors (e.g. the
        // __cxa_thread_atexit fallback in libc++abi).
        array->skip_destructor_rounds--;
        emutls_setspecific(array);
    } else {
        emutls_shutdown(array);
        FreeVec(ptr);
    }
}

static __inline void emutls_init(void) {
    if (_pthread_key_create(&emutls_pthread_key, emutls_key_destructor) != 0)
        abort();

    emutls_key_created = true;
}

static __inline void emutls_init_once(void) {
    Printf("emutls_init_once");

    ThreadInfo *inf = &threads[0];

    set_tls_register(inf);
    emutls_init();
}

static __inline void emutls_lock(void) { MutexObtain(emutls_mutex); }
static __inline void emutls_unlock(void) { MutexRelease(emutls_mutex); }

static size_t emutls_num_object = 0; // number of allocated TLS objects

// Free the allocated TLS data
static void emutls_shutdown(emutls_address_array *array) {
    if (array) {
        uintptr_t i;
        for (i = 0; i < array->size; ++i) {
            if (array->data[i])
                emutls_memalign_free(array->data[i]);
        }
    }
}

// For every TLS variable xyz,
// there is one __emutls_control variable named __emutls_v.xyz.
// If xyz has non-zero initial value, __emutls_v.xyz's "value"
// will point to __emutls_t.xyz, which has the initial value.
typedef struct __emutls_control {
    // Must use gcc_word here, instead of size_t, to match GCC.  When
    // gcc_word is larger than size_t, the upper extra bits are all
    // zeros.  We can use variables of size_t to operate on size and
    // align.
    gcc_word size;  // size of the object in bytes
    gcc_word align; // alignment of the object in bytes
    union {
        uintptr_t index; // data[index-1] is the object address
        void *address;   // object address, when in single thread env
    } object;
    void *value; // null or non-zero initial value for the object
} __emutls_control;

// Emulated TLS objects are always allocated at run-time.
static __inline void *emutls_allocate_object(__emutls_control *control) {
    size_t size = control->size;
    size_t align = control->align;
    DECLARE_UTILITYBASE();

    void *base;
    if (align < sizeof(void *))
        align = sizeof(void *);
    // Make sure that align is power of 2.
    if ((align & (align - 1)) != 0)
        abort();

    base = emutls_memalign_alloc(align, size);
    if (control->value)
        CopyMem(control->value, base, size);
    else
        ClearMem(base, size);
    return base;
}

// Returns control->object.index; set index if not allocated yet.
static __inline uintptr_t emutls_get_index(__emutls_control *control) {
    uintptr_t index = __atomic_load_n(&control->object.index, __ATOMIC_ACQUIRE);
    if (!index) {
        emutls_init_once();
        emutls_lock();
        index = control->object.index;
        if (!index) {
            index = ++emutls_num_object;
            __atomic_store_n(&control->object.index, index, __ATOMIC_RELEASE);
        }
        emutls_unlock();
    }
    return index;
}

// Updates newly allocated thread local emutls_address_array.
static __inline void emutls_check_array_set_size(emutls_address_array *array, uintptr_t size) {
    if (array == NULL)
        abort();
    array->size = size;
    emutls_setspecific(array);
}

// Returns the new 'data' array size, number of elements,
// which must be no smaller than the given index.
static __inline uintptr_t emutls_new_data_array_size(uintptr_t index) {
    // Need to allocate emutls_address_array with extra slots
    // to store the header.
    // Round up the emutls_address_array size to multiple of 16.
    uintptr_t header_words = sizeof(emutls_address_array) / sizeof(void *);
    return ((index + header_words + 15) & ~((uintptr_t) 15)) - header_words;
}

// Returns the size in bytes required for an emutls_address_array with
// N number of elements for data field.
static __inline uintptr_t emutls_asize(uintptr_t N) {
    return N * sizeof(void *) + sizeof(emutls_address_array);
}

// Returns the thread local emutls_address_array.
// Extends its size if necessary to hold address at index.
static __inline emutls_address_array *
emutls_get_address_array(uintptr_t index) {
    emutls_address_array *array = emutls_getspecific();
    DECLARE_UTILITYBASE();

    if (array == NULL) {
        uintptr_t new_size = emutls_new_data_array_size(index);
        array = (emutls_address_array *) AllocVecTags(emutls_asize(new_size), AVT_Type, MEMF_SHARED, TAG_DONE);
        if (array) {
            ClearMem(array->data, new_size * sizeof(void *));
            array->skip_destructor_rounds = 0;
        }
        emutls_check_array_set_size(array, new_size);
    } else if (index > array->size) {
        uintptr_t orig_size = array->size;
        uintptr_t new_size = emutls_new_data_array_size(index);
        array = (emutls_address_array *) realloc(array, emutls_asize(new_size));
        if (array)
            ClearMem(array->data + orig_size, (new_size - orig_size) * sizeof(void *));
        emutls_check_array_set_size(array, new_size);
    }
    return array;
}

// Our emulated TLS implementation relies on local state (e.g. for the pthread
// key), and if we duplicate this state across different shared libraries,
// accesses to the same TLS variable from different shared libraries will yield
// different results (see https://github.com/android/ndk/issues/1551 for an
// example). __emutls_get_address is the only external entry point for emulated
// TLS, and by making it default visibility and weak, we can rely on the dynamic
// linker to coalesce multiple copies at runtime and ensure a single unique copy
// of TLS state. This is a best effort; it won't work if the user is linking
// with -Bsymbolic or -Bsymbolic-functions, and it also won't work on Windows,
// where the dynamic linker has no notion of coalescing weak symbols at runtime.
// A more robust solution would be to create a separate shared library for
// emulated TLS, to ensure a single copy of its state.
__attribute__((visibility("default"), weak)) void *__emutls_get_address(__emutls_control *control);

__attribute__((visibility("default"), weak))
void *__emutls_get_address(__emutls_control *control) {
    uintptr_t index = emutls_get_index(control);
    emutls_address_array *array = emutls_get_address_array(index--);
    if (array->data[index] == NULL)
        array->data[index] = emutls_allocate_object(control);
    return array->data[index];
}
