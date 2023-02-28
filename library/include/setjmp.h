/*
 * $Id: setjmp.h,v 1.5 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _SETJMP_H
#define _SETJMP_H

#include <features.h>

#include <signal.h>

__BEGIN_DECLS

struct __jmp_buf {
    void *jb_ReturnAddress;
    unsigned long jb_CondCode;
    void *jb_StackPointer;
    unsigned long jb_GPR[19];
    double jb_FPR[18];
};

typedef struct __jmp_buf jmp_buf[1];

struct __jmp_buf_tag {
    /* NOTE: The machine-dependent definitions of `__sigsetjmp'
       assume that a `jmp_buf' begins with a `__jmp_buf' and that
       `__mask_was_saved' follows it.  Do not move these members
       or add others before it. */
    jmp_buf __jmpbuf;       /* Calling environment.  */
    int __mask_was_saved;     /* Saved the signal mask?  */
    sigset_t __saved_mask;    /* Saved signal mask.  */
};

typedef struct __jmp_buf_tag sigjmp_buf[1];

extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int status);
extern void siglongjmp(sigjmp_buf buf, int ret);

__END_DECLS

#endif /* _SETJMP_H */
