/*
 * $Id: setjmp.h,v 1.5 2006-01-08 12:0__sigset_t6:14 clib2devs Exp $
*/

#ifndef _SETJMP_H
#define _SETJMP_H

#include <features.h>

#include <signal.h>

__BEGIN_DECLS

typedef unsigned long long __jmp_buf[58];

/* Calling environment, plus possibly a saved signal mask.  */
struct __jmp_buf_tag {
    /* NOTE: The machine-dependent definitions of `__sigsetjmp'
       assume that a `jmp_buf' begins with a `__jmp_buf' and that
       `__mask_was_saved' follows it.  Do not move these members
       or add others before it.  */
    __jmp_buf __jmpbuf;		/* Calling environment.  */
    int __mask_was_saved;	/* Saved the signal mask?  */
    sigset_t __saved_mask;	/* Saved signal mask.  */
};

typedef struct __jmp_buf_tag jmp_buf[1];

/* Store the calling environment in ENV, also saving the signal mask. Return 0.  */
extern int setjmp(jmp_buf __env);

/* Store the calling environment in ENV, also saving the
   signal mask if SAVEMASK is nonzero.  Return 0.
   This is the internal name for `sigsetjmp'.  */
extern int __sigsetjmp(struct __jmp_buf_tag __env[1], int __savemask);

/* Store the calling environment in ENV, not saving the signal mask. Return 0.  */
extern int _setjmp(struct __jmp_buf_tag __env[1]);

/* Jump to the environment saved in ENV, making the `setjmp' call there return VAL, or 1 if VAL is 0.  */
extern void longjmp(jmp_buf __env, int __val) __attribute__ ((__noreturn__));

#if defined __USE_MISC || defined __USE_XOPEN
/* Same.  Usually `_longjmp' is used with `_setjmp', which does not save
   the signal mask.  But it is how ENV was saved that determines whether
   `longjmp' restores the mask; `_longjmp' is just an alias.  */
extern void _longjmp(struct __jmp_buf_tag __env[1], int __val) __attribute__ ((__noreturn__));
#endif

#ifdef	__USE_POSIX
/* Use the same type for `jmp_buf' and `sigjmp_buf'.
   The `__mask_was_saved' flag determines whether
   or not `longjmp' will restore the signal mask.  */
typedef struct __jmp_buf_tag sigjmp_buf[1];

/* Store the calling environment in ENV, also saving the
   signal mask if SAVEMASK is nonzero.  Return 0.  */
# define sigsetjmp(env, savemask)	__sigsetjmp (env, savemask)

/* Jump to the environment saved in ENV, making the
   sigsetjmp call there return VAL, or 1 if VAL is 0.
   Restore the signal mask if that sigsetjmp call saved it.
   This is just an alias `longjmp'.  */
extern void siglongjmp (sigjmp_buf __env, int __val) __attribute__ ((__noreturn__));
#endif /* Use POSIX.  */

extern int __sigjmp_save (jmp_buf __env, int __savemask);

__END_DECLS

#endif /* _SETJMP_H */
