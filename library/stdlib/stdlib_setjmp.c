/*
 * $Id: stdlib_setjmp.c,v 1.6 2010-10-20 13:50:17 obarthel Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
	blr						\n\
							\n\
");

__asm("						\n\
	.text					\n\
	.align	2				\n\
							\n\
	.globl	longjmp			\n\
							\n\
longjmp:					\n\
							\n\
	lmw		r10,0(r3)		\n\
	mtlr	r10				\n\
	mtcr	r11				\n\
	mr		r1,r12			\n\
	lfd		f14, 88(r3)		\n\
	lfd		f15, 96(r3)		\n\
	lfd		f16, 104(r3)	\n\
	lfd		f17, 112(r3)	\n\
	lfd		f18, 120(r3)	\n\
	lfd		f19, 128(r3)	\n\
	lfd		f20, 136(r3)	\n\
	lfd		f21, 144(r3)	\n\
	lfd		f22, 152(r3)	\n\
	lfd		f23, 160(r3)	\n\
	lfd		f24, 168(r3)	\n\
	lfd		f25, 176(r3)	\n\
	lfd		f26, 184(r3)	\n\
	lfd		f27, 192(r3)	\n\
	lfd		f28, 200(r3)	\n\
	lfd		f29, 208(r3)	\n\
	lfd		f30, 216(r3)	\n\
	lfd		f31, 224(r3)	\n\
	cmpwi	r4, 0			\n\
	bne		1f				\n\
	li		r3,1			\n\
	blr						\n\
1:							\n\
	mr		r3, r4			\n\
	blr						\n\
							\n\
");
