/*
 * setjmp.S - setjmp, longjmp and the signal variants, for ARM (AAPCS).
 *
 * The environment is the 464-byte __jmp_buf (58 doublewords) followed by the
 * __mask_was_saved word that sigjmp.c maintains. On ARM the callee-saved state
 * is r4-r11, sp and lr; the words used are:
 *      0..28   r4..r11
 *     32       sp
 *     36       lr      (the return address of the setjmp call)
 * Soft-float: no VFP register is callee-saved state the library relies on, so
 * none is saved. The rest of the __jmp_buf is unused on ARM.
 *
 * setjmp returns 0. longjmp returns its value, or 1 when that value is 0, as
 * the C standard requires. __sigsetjmp leaves the signal mask to __sigjmp_save,
 * the C routine that also records __mask_was_saved, and returns its result (0).
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

	.syntax unified
	.arm
	.text

	.globl	setjmp
	.type	setjmp, %function
setjmp:
	stmia	r0, {r4-r11}
	str	sp, [r0, #32]
	str	lr, [r0, #36]
	mov	r0, #0
	bx	lr
	.size	setjmp, .-setjmp

	.globl	_setjmp
	.type	_setjmp, %function
_setjmp:
	/* No signal mask saved: __sigjmp_save records savemask 0. */
	mov	r1, #0
	b	__sigsetjmp
	.size	_setjmp, .-_setjmp

	.globl	__sigsetjmp
	.type	__sigsetjmp, %function
__sigsetjmp:
	stmia	r0, {r4-r11}
	str	sp, [r0, #32]
	str	lr, [r0, #36]
	/* Tail call: __sigjmp_save(env, savemask) sets __mask_was_saved and
	 * returns 0, which goes straight back to our caller. */
	b	__sigjmp_save
	.size	__sigsetjmp, .-__sigsetjmp

	.globl	longjmp
	.type	longjmp, %function
longjmp:
	ldmia	r0, {r4-r11}
	ldr	sp, [r0, #32]
	ldr	lr, [r0, #36]
	cmp	r1, #0
	moveq	r1, #1
	mov	r0, r1
	bx	lr
	.size	longjmp, .-longjmp

	.globl	_longjmp
	.type	_longjmp, %function
_longjmp:
	b	longjmp
	.size	_longjmp, .-_longjmp

	.section	.note.GNU-stack,"",%progbits
