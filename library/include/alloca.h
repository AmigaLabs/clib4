#ifndef _ALLOCA_H
#define _ALLOCA_H       1

#define __need_size_t
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/*
 * You can request to use the alloca() variant that actually does allocate
 * memory from the system rather than the current stack frame, which will
 * ease stack requirements but may not release allocate memory immediately.
 * In order to do so, add #define __USE_CLIB2_ALLOCA to your program, or
 * -D__USE_CLIB2_ALLOCA to your build makefile. For GCC you will also have
 * to compile your program with the -fno-builtin switch to override the
 * built-in alloca() function.
 */

#if defined(alloca)
#undef alloca
#endif /* alloca */

#if defined(__GNUC__) && !defined(__USE_CLIB2_ALLOCA)
 #define alloca(size) __builtin_alloca(size)
#else
 extern void * alloca(size_t size);

 #ifdef __MEM_DEBUG
  extern void * __alloca(size_t size,const char *file,int line);

  #define alloca(size) __alloca((size),__FILE__,__LINE__)
 #else
  /* This is necessary because some applications key off the fact that
     alloca is a symbol defined by the preprocessor. */
  #define alloca alloca
 #endif /* __MEM_DEBUG */
#endif /* __GNUC__ */

__END_DECLS

#endif /* alloca.h */