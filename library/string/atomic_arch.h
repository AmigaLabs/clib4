/*
 * ARM (ARMv7-A): the compiler's __sync builtins compile to ldrex/strex loops
 * with the barriers the architecture needs, so the primitives are written
 * against them. No load-linked/store-conditional pair is exposed; atomic.h
 * builds everything else from a_cas, a_barrier and a_clz_32.
 */
#if defined(__arm__)
#define a_cas a_cas
static inline int a_cas(volatile int *p, int t, int s) {
    return __sync_val_compare_and_swap(p, t, s);
}

#define a_barrier a_barrier
static inline void a_barrier(void) {
    __sync_synchronize();
}

#define a_clz_32 a_clz_32
static inline int a_clz_32(uint32_t x) {
    return x ? __builtin_clz(x) : 32;
}
#else /* PowerPC */
#define a_ll a_ll
static inline int a_ll(volatile int *p) {
    int v;
    __asm__ __volatile__ ("lwarx %0, 0, %2" : "=r"(v) : "m"(*p), "r"(p));
    return v;
}

#define a_sc a_sc
static inline int a_sc(volatile int *p, int v) {
    int r;
    __asm__ __volatile__ (
            "stwcx. %2, 0, %3 ; mfcr %0"
            : "=r"(r), "=m"(*p) : "r"(v), "r"(p) : "memory", "cc");
    return r & 0x20000000; /* "bit 2" of "cr0" (backwards bit order) */
}

#define a_barrier a_barrier
static inline void a_barrier() {
    __asm__ __volatile__ ("sync" : : : "memory");
}
#define a_pre_llsc a_barrier

#define a_post_llsc a_post_llsc
static inline void a_post_llsc() {
    __asm__ __volatile__ ("isync" : : : "memory");
}

#define a_clz_32 a_clz_32
static inline int a_clz_32(uint32_t x) {
    __asm__ ("cntlzw %0, %1" : "=r"(x) : "r"(x));
    return x;
}
#endif /* __arm__ */
