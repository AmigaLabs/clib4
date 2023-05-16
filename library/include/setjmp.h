/*
 * $Id: setjmp.h,v 1.5 2006-01-08 12:0__sigset_t6:14 clib2devs Exp $
*/

#ifndef _SETJMP_H
#define _SETJMP_H

#include <features.h>
#include <signal.h>

__BEGIN_DECLS

#if defined(__ALTIVEC__)
    #define _JBLEN 64
#else
    #define _JBLEN 32
#endif
#define _JBTYPE double

#ifdef _JBLEN
    #ifdef _JBTYPE
        typedef	_JBTYPE jmp_buf[_JBLEN];
    #else
        typedef	int jmp_buf[_JBLEN];
    #endif
#endif

/* POSIX sigsetjmp/siglongjmp macros */
typedef int sigjmp_buf[_JBLEN+2];

#define _SAVEMASK	_JBLEN
#define _SIGMASK	(_JBLEN+1)

/* Store the calling environment in ENV, also saving the signal mask. Return 0.  */
extern int setjmp(jmp_buf __env);

/* Jump to the environment saved in ENV, making the `setjmp' call there return VAL, or 1 if VAL is 0.  */
extern void longjmp(jmp_buf __env, int __val);

#define sigsetjmp(env, savemask) \
            __extension__ \
            ({ \
              sigjmp_buf *_sjbuf = &(env); \
              ((*_sjbuf)[_SAVEMASK] = savemask,\
              sigprocmask (SIG_SETMASK, 0, (sigset_t *)((*_sjbuf) + _SIGMASK)),\
              setjmp (*_sjbuf)); \
            })

#define siglongjmp(env, val) \
            __extension__ \
            ({ \
              sigjmp_buf *_sjbuf = &(env); \
              ((((*_sjbuf)[_SAVEMASK]) ? \
               sigprocmask (SIG_SETMASK, (sigset_t *)((*_sjbuf) + _SIGMASK), 0)\
               : 0), \
               longjmp (*_sjbuf, val)); \
            })

__END_DECLS

#endif /* _SETJMP_H */
