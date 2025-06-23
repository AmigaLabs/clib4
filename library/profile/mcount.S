/*
/ $Id: profile_mcount.S,v 1.1 2023-10-20 12:04:26 clib4devs Exp $
*/

/* We do profiling as described in the SYSV ELF ABI, except that glibc
   _mcount manages its own counters.  The caller has put the address the
   caller will return to in the usual place on the stack, 4(r1).  _mcount
   is responsible for ensuring that when it returns no argument-passing
   registers are disturbed, and that the LR is set back to (what the
   caller sees as) 4(r1).

   This is intended so that the following code can be inserted at the
   front of any routine without changing the routine:

	.data
	mflr	r0
	stw	r0,4(r1)
	bl	_mcount
*/

#ifdef PIC
# define JUMPTARGET(name) name##@plt
#else
# define JUMPTARGET(name) name
#endif

    .text
    .global _mcount
    .type   _mcount, @function
_mcount:
    .cfi_startproc
	stwu	r1,-48(r1)
	.cfi_adjust_cfa_offset 48
/* We need to save the parameter-passing registers.  */
	stw	r3, 12(r1)
	stw	r4, 16(r1)
	stw	r5, 20(r1)
	stw	r6, 24(r1)
	mflr	r4
	lwz	r3, 52(r1)
	mfcr	r5
	stw	r7, 28(r1)
	stw	r8, 32(r1)
	stw	r9, 36(r1)
	stw	r10,40(r1)
	stw	r4, 44(r1)
	.cfi_offset lr, -4
	stw	r5,  8(r1)
	bl	JUMPTARGET(__mcount)
 /* Restore the registers...  */
	lwz     r6,  8(r1)
	lwz	r0, 44(r1)
	lwz	r3, 12(r1)
	mtctr	r0
	lwz	r4, 16(r1)
	mtcrf	0xff,r6
	lwz	r5, 20(r1)
	lwz	r6, 24(r1)
	lwz	r0, 52(r1)
	lwz	r7, 28(r1)
	lwz	r8, 32(r1)
	mtlr	r0
	lwz	r9, 36(r1)
	lwz	r10,40(r1)
 /* ...unwind the stack frame, and return to your usual programming.  */
	addi	r1,r1,48
	bctr

    .cfi_endproc
    .size _mcount,.-_mcount
