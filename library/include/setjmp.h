/*
 * $Id: setjmp.h,v 1.5 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _SETJMP_H
#define _SETJMP_H

#include <features.h>

#include <signal.h>

__BEGIN_DECLS

typedef unsigned long long __jmp_buf[56];

typedef struct __jmp_buf_tag {
    __jmp_buf __jb;
    unsigned long __fl;
    unsigned long __ss[128/sizeof(long)];
} jmp_buf[1];

typedef __jmp_buf_tag sigjmp_buf;

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)
#define __setjmp_attr __attribute__((__returns_twice__))
#else
#define __setjmp_attr
#endif

extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int status);
extern void siglongjmp(sigjmp_buf buf, int ret);
extern int sigsetjmp (sigjmp_buf buf, int ret) __setjmp_attr;

extern int _setjmp (jmp_buf buf) __setjmp_attr;
extern void _longjmp (jmp_buf buf, int ret);

#undef __setjmp_attr

__END_DECLS

#endif /* _SETJMP_H */
