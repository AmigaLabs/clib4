#ifndef DECLS
#define DECLS

#include <stddef.h>

#ifdef __cplusplus

using memset_ty = void *(void *s, int c, size_t n);
using memcpy_ty = void *(void *dest, const void *src, size_t n);

extern "C" {
#endif

void *__folly_memcpy(void *dest, const void *src, size_t n);
void *libc_memcpy(void *dest, const void *src, size_t n);
void *local_memcpy(void *dest, const void *src, size_t n);

void *libc_memset(void *s, int c, size_t n);
void *local_memset(void *s, int c, size_t n);
void *musl_memset(void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif

#endif // DECLS