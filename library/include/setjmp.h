/*
 * $Id: setjmp.h,v 1.5 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _SETJMP_H
#define _SETJMP_H

#include <features.h>

__BEGIN_DECLS

struct __jmp_buf
{
	void *         jb_ReturnAddress;
	unsigned long  jb_CondCode;
	void *         jb_StackPointer;
	unsigned long  jb_GPR[19];
	double         jb_FPR[18];
};

typedef struct __jmp_buf jmp_buf[1];

extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env,int status);

__END_DECLS

#endif /* _SETJMP_H */
