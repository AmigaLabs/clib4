/*
 * $Id: stdlib_setjmp.c,v 1.6 2010-10-20 13:50:17 clib2devs Exp $
*/

__asm("					\n\
						\n\
	.text				\n\
	.align	2			\n\
						\n\
	.globl	setjmp		\n\
						\n\
setjmp:					\n\
                        \n\
	addi	3,3,7		\n\
	rlwinm	3,3,0,0,28	\n\
	stw	1,0(3)			\n\
	stwu	2,4(3)		\n\
	stwu	13,4(3)		\n\
	stwu	14,4(3)		\n\
	stwu	15,4(3)		\n\
	stwu	16,4(3)		\n\
	stwu	17,4(3)		\n\
	stwu	18,4(3)		\n\
	stwu	19,4(3)		\n\
	stwu	20,4(3)		\n\
	stwu	21,4(3)		\n\
	stwu	22,4(3)		\n\
	stwu	23,4(3)		\n\
	stwu	24,4(3)		\n\
	stwu	25,4(3)		\n\
	stwu	26,4(3)		\n\
	stwu	27,4(3)		\n\
	stwu	28,4(3)		\n\
	stwu	29,4(3)		\n\
	stwu	30,4(3)		\n\
	stwu	31,4(3)		\n\
	mflr	4           \n\
	stwu	4,4(3)		\n\
	mfcr	4           \n\
	stwu	4,4(3)		\n\
	stfdu	14,8(3)		\n\
	stfdu	15,8(3)		\n\
	stfdu	16,8(3)		\n\
	stfdu	17,8(3)		\n\
	stfdu	18,8(3)		\n\
	stfdu	19,8(3)		\n\
	stfdu	20,8(3)		\n\
	stfdu	21,8(3)		\n\
	stfdu	22,8(3)		\n\
	stfdu	23,8(3)		\n\
	stfdu	24,8(3)		\n\
	stfdu	25,8(3)		\n\
	stfdu	26,8(3)		\n\
	stfdu	27,8(3)		\n\
	stfdu	28,8(3)		\n\
	stfdu	29,8(3)		\n\
	stfdu	30,8(3)		\n\
	stfdu	31,8(3)		\n\
	li	3,0				\n\
	blr					\n\
                        \n\
");

__asm("					\n\
	.text				\n\
	.align	2			\n\
						\n\
	.globl	longjmp		\n\
						\n\
longjmp:				\n\
						\n\
	addi	3,3,7		\n\
	rlwinm	3,3,0,0,28  \n\
	lwz	1,0(3)		    \n\
	lwzu	2,4(3)	    \n\
	lwzu	13,4(3)	    \n\
	lwzu	14,4(3)	    \n\
	lwzu	15,4(3)	    \n\
	lwzu	16,4(3)	    \n\
	lwzu	17,4(3)	    \n\
	lwzu	18,4(3)	    \n\
	lwzu	19,4(3)	    \n\
	lwzu	20,4(3)	    \n\
	lwzu	21,4(3)	    \n\
	lwzu	22,4(3)	    \n\
	lwzu	23,4(3)	    \n\
	lwzu	24,4(3)	    \n\
	lwzu	25,4(3)	    \n\
	lwzu	26,4(3)	    \n\
	lwzu	27,4(3)	    \n\
	lwzu	28,4(3)	    \n\
	lwzu	29,4(3)	    \n\
	lwzu	30,4(3)	    \n\
	lwzu	31,4(3)	    \n\
	lwzu	5,4(3)	    \n\
	mtlr	5           \n\
	lwzu	5,4(3)	    \n\
	mtcrf	255,5       \n\
	lfdu	14,8(3)     \n\
	lfdu	15,8(3)     \n\
	lfdu	16,8(3)     \n\
	lfdu	17,8(3)     \n\
	lfdu	18,8(3)     \n\
	lfdu	19,8(3)     \n\
	lfdu	20,8(3)     \n\
	lfdu	21,8(3)     \n\
	lfdu	22,8(3)     \n\
	lfdu	23,8(3)     \n\
	lfdu	24,8(3)     \n\
	lfdu	25,8(3)     \n\
	lfdu	26,8(3)     \n\
	lfdu	27,8(3)     \n\
	lfdu	28,8(3)     \n\
	lfdu	29,8(3)     \n\
	lfdu	30,8(3)     \n\
	lfdu	31,8(3)     \n\
	mr.	3,4             \n\
	bclr+	4,2         \n\
	li	3,1             \n\
	blr					\n\
");
