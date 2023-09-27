/*
 * $Id: stdatomic.h, v1.0 2022-03-22 13:12:59 clib4devs Exp $
*/

#if __STDC_VERSION__ >= 200112L && \
    defined(__GNUC__) && !defined(__STDC_NO_ATOMICS__)

#ifndef _STDATOMIC_H
#define _STDATOMIC_H

#include <features.h>

#ifndef _UCHAR_H
#include <uchar.h>
#endif /* _UCHAR_H */

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif /* _STDBOOL_H */

typedef _Atomic(_Bool) atomic_bool;
typedef _Atomic(char) atomic_char;
typedef _Atomic(signed char) atomic_schar;
typedef _Atomic(unsigned char) atomic_uchar;
typedef _Atomic(short) atomic_short;
typedef _Atomic(unsigned short) atomic_ushort;
typedef _Atomic(int) atomic_int;
typedef _Atomic(unsigned int) atomic_uint;
typedef _Atomic(long) atomic_long;
typedef _Atomic(unsigned long) atomic_ulong;
typedef _Atomic(long long) atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(char16_t) atomic_char16_t;
typedef _Atomic(char32_t) atomic_char32_t;
typedef _Atomic(wchar_t) atomic_wchar_t;
typedef _Atomic(int_least8_t) atomic_int_least8_t;
typedef _Atomic(uint_least8_t) atomic_uint_least8_t;
typedef _Atomic(int_least16_t) atomic_int_least16_t;
typedef _Atomic(uint_least16_t) atomic_uint_least16_t;
typedef _Atomic(int_least32_t) atomic_int_least32_t;
typedef _Atomic(uint_least32_t) atomic_uint_least32_t;
typedef _Atomic(int_least64_t) atomic_int_least64_t;
typedef _Atomic(uint_least64_t) atomic_uint_least64_t;
typedef _Atomic(int_fast8_t) atomic_int_fast8_t;
typedef _Atomic(uint_fast8_t) atomic_uint_fast8_t;
typedef _Atomic(int_fast16_t) atomic_int_fast16_t;
typedef _Atomic(uint_fast16_t) atomic_uint_fast16_t;
typedef _Atomic(int_fast32_t) atomic_int_fast32_t;
typedef _Atomic(uint_fast32_t) atomic_uint_fast32_t;
typedef _Atomic(int_fast64_t) atomic_int_fast64_t;
typedef _Atomic(uint_fast64_t) atomic_uint_fast64_t;
typedef _Atomic(intptr_t) atomic_intptr_t;
typedef _Atomic(uintptr_t) atomic_uintptr_t;
typedef _Atomic(size_t) atomic_size_t;
typedef _Atomic(ptrdiff_t) atomic_ptrdiff_t;
typedef _Atomic(intmax_t) atomic_intmax_t;
typedef _Atomic(uintmax_t) atomic_uintmax_t;

typedef struct
{
    atomic_bool	__flag;
}   atomic_flag;

typedef enum
{
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
}   memory_order;

static inline bool
atomic_flag_test_and_set_explicit(volatile atomic_flag *, memory_order)
__attribute__((always_inline));

static inline bool
atomic_flag_test_and_set(volatile atomic_flag *)
__attribute__((always_inline));

static inline void
atomic_flag_clear_explicit(volatile atomic_flag *, memory_order)
__attribute__((always_inline));

static inline void
atomic_flag_clear(volatile atomic_flag *)
__attribute__((always_inline));

#ifdef __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_BOOL_LOCK_FREE __GCC_ATOMIC_BOOL_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE __GCC_ATOMIC_CHAR_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE __GCC_ATOMIC_SHORT_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE __GCC_ATOMIC_INT_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE __GCC_ATOMIC_LONG_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE __GCC_ATOMIC_LLONG_LOCK_FREE
#endif
#ifdef __GCC_ATOMIC_POINTER_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE __GCC_ATOMIC_POINTER_LOCK_FREE
#endif

#define ATOMIC_VAR_INIT(VALUE) (VALUE)
#define	ATOMIC_FLAG_INIT { ATOMIC_VAR_INIT(0) }

#define atomic_is_lock_free(O) __atomic_is_lock_free(sizeof (*(O)), (O))
#define	atomic_store_explicit(O, D, S) __atomic_store_n(O, D, S)
#define atomic_store(O, D) atomic_store_explicit(O, D, memory_order_seq_cst)
#define atomic_init(O, V) atomic_store_explicit(O, V, memory_order_relaxed)
#define	atomic_load_explicit(O, S) __atomic_load_n(O, S)
#define atomic_load(O) atomic_load_explicit(O, memory_order_seq_cst)
#define	atomic_exchange_explicit(O, D, S) __atomic_exchange_n(O, D, S)
#define atomic_exchange(O, D) atomic_exchange_explicit(O, D, memory_order_seq_cst)
#define	atomic_compare_exchange_strong_explicit(O, E, D, S, F) \
      __atomic_compare_exchange_n(O, E, D, 0, S, F)
#define atomic_compare_exchange_strong(O, E, D) \
        atomic_compare_exchange_strong_explicit(O, E, D, memory_order_seq_cst, memory_order_seq_cst)
#define	atomic_compare_exchange_weak_explicit(O, E, D, S, F) \
      __atomic_compare_exchange_n(O, E, D, 1, S, F)
#define atomic_compare_exchange_weak(O, E, D) \
        atomic_compare_exchange_weak_explicit(O, E, D, memory_order_seq_cst, memory_order_seq_cst)
#define	atomic_fetch_add_explicit(O, P, S) __atomic_fetch_add(O, P, S)
#define atomic_fetch_add(O, P) atomic_fetch_add_explicit((O), (P), memory_order_seq_cst)
#define	atomic_fetch_sub_explicit(O, P, S) __atomic_fetch_sub(O, P, S)
#define atomic_fetch_sub(P, V) atomic_fetch_sub ((P), (V), memory_order_seq_cst)
#define	atomic_fetch_or_explicit(O, P, S) __atomic_fetch_or(O, P, S)
#define	atomic_fetch_or(O, P) atomic_fetch_or_explicit(O, P, memory_order_seq_cst)
#define	atomic_fetch_xor_explicit(O, P, S) __atomic_fetch_xor(O, P, S)
#define	atomic_fetch_xor(O, P) atomic_fetch_xor_explicit(O, P, memory_order_seq_cst)
#define	atomic_fetch_and_explicit(O, P, S) __atomic_fetch_and(O, P, S)
#define	atomic_fetch_and(O, P) atomic_fetch_and_explicit(O, P, memory_order_seq_cst)
#define atomic_thread_fence(S) __atomic_thread_fence(S)
#define atomic_signal_fence(S) __atomic_signal_fence(S)
#define kill_dependency(Y) __extension__ ({ __auto_type __kill_dependency_tmp = (Y); \
      __kill_dependency_tmp; })

static inline bool
atomic_flag_test_and_set_explicit(volatile atomic_flag *obj, memory_order ord)
{
    return atomic_exchange_explicit(&obj->__flag, 1, ord);
}

static inline bool
atomic_flag_test_and_set(volatile atomic_flag *obj)
{
    return atomic_flag_test_and_set_explicit(obj, memory_order_seq_cst);
}

static inline void
atomic_flag_clear_explicit(volatile atomic_flag *obj, memory_order ord)
{
    atomic_store_explicit(&obj->__flag, 0, ord);
}

static inline void
atomic_flag_clear(volatile atomic_flag *obj)
{
    atomic_flag_clear_explicit(obj, memory_order_seq_cst);
}

#endif /* _STDATOMIC_H */
#endif /* __STDC_VERSION__ >= 200112L && \
          defined(__GNUC__) && !defined(__STDC_NO_ATOMICS__) */
