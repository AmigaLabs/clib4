/*
 * $Id: stdlib_setjmp.c,v 1.6 2010-10-20 13:50:17 clib2devs Exp $
 */

__asm("						\n\
							\n\
	.text					\n\
	.align	2				\n\
							\n\
	.globl	setjmp			\n\
							\n\
setjmp:						\n\
							\n\
	mflr	r10				\n\
	mfcr	r11				\n\
	mr		r12, r1			\n\
	stmw	r10, 0(r3)		\n\
	stfd	f14, 88(r3)		\n\
	stfd	f15, 96(r3)		\n\
	stfd	f16, 104(r3)	\n\
	stfd	f17, 112(r3)	\n\
	stfd	f18, 120(r3)	\n\
	stfd	f19, 128(r3)	\n\
	stfd	f20, 136(r3)	\n\
	stfd	f21, 144(r3)	\n\
	stfd	f22, 152(r3)	\n\
	stfd	f23, 160(r3)	\n\
	stfd	f24, 168(r3)	\n\
	stfd	f25, 176(r3)	\n\
	stfd	f26, 184(r3)	\n\
	stfd	f27, 192(r3)	\n\
	stfd	f28, 200(r3)	\n\
	stfd	f29, 208(r3)	\n\
	stfd	f30, 216(r3)	\n\
	stfd	f31, 224(r3)	\n\
	li		r3, 0			\n\
    blr                     \n\
							\n\
");
