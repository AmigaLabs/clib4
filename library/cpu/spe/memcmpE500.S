/*------------------------------------------------------------------
 * memcmp.S
 *
 * Standard memcmp function optimized for e500 using SPE.
 *
 *      Copyright (c) 2005 Freescale Semiconductor, Inc
 *      ALL RIGHTS RESERVED
 *
 *	Redistribution and use in source and binary forms, with or
 *	without modification, are permitted provided that the following
 *	conditions are met:
 *
 *
 *	Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *
 *	Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *
 *	Neither the name of Freescale Semiconductor, Inc nor the names of
 *	its contributors may be used to endorse or promote products derived
 *	from this software without specific prior written permission.
 *
 *
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *	TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *	ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *	OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *	POSSIBILITY OF SUCH DAMAGE.
 *
 *------------------------------------------------------------------
 */

/*------------------------------------------------------------------
 * int memcmp(unsigned char *s1, unsigned char *s2, unsigned int len)
 *
 * Returns:
 *  0 if equal
 *  <0 if less
 *  >0 if greater
 *------------------------------------------------------------------
 */

	.file	"memcmp.S"
	.section	".text"
	.align 4
	#define FUNCTION e500_memcmp
	.globl FUNCTION
	.type FUNCTION,@function
FUNCTION:
        or r0,r3,r4
        andi. r0,r0,7
        xor r0,r3,r4
        cmpli cr1,r5,32
        mtcrf 0x1,r5
        bne memcmp_unaligned

aligned_copy:
        mtcrf 0x2,r5
        bge cr1,big_aligned

        /* We check the last 0-31 bytes in powers of 2, starting
         * with 16 bytes (two doubles), and moving on down until
         * all the bytes have been compared, or a difference has
         * been found.
         */
try_two_doubles:
        bf 27,try_one_double
        evldd r6,0(r3)
        evldd r10,0(r4)
        evldd r7,8(r3)
        addi r3,r3,16
        evldd r11,8(r4)
        addi r4,r4,16
        evcmpeq 1,r6,r10
        bf 7,found_diff1
        evcmpeq 5,r7,r11
        bf 23,found_diff2

try_one_double:
        bf 28,try_word
        evldd r6,0(r3)
        addi r3,r3,8
        evldd r10,0(r4)
        addi r4,r4,8
        evcmpeq 1,r6,r10
        bf 7,found_diff1

try_word:
        cror 31,30,31
        bf 29,do_last
        lwz r6,0(r3)
        addi r3,r3,4
        lwz r10,0(r4)
        addi r4,r4,4
        subfc. r0,r6,r10
        bne found_word_diff

        /*  Here we mask off any bytes in the last word that we
         *  don't want to look at (they are outside the buffer),
         *  and then we compare the two words.
         */
do_last:
        bf 31,finish
        li r8,-1
        lwz r6,0(r3)
        rlwinm r5,r5,3,27,28
        lwz r10,0(r4)
        srw r8,r8,r5
        or r6,r6,r8
        or r10,r10,r8
        subfc. r0,r6,r10
        bne found_word_diff

        /* We only come here if no differences were found. */
finish:
        li r3,0
        blr

        /* If we found a difference while comparing words, take
         * the overflow from the subfc., then OR 1 with it, to make
         * sure we return 1 or -1 (and not zero)
         */
found_word_diff:
        subfe r3,r3,r3 /* will be zero or -1 */
        nand r3,r3,r3
        ori r3,r3,1
        blr

big_aligned:
        /* At this point, we know there are at LEAST 32 bytes of
         * data to compare, so have fun!
         */
        evldd r6,0(r3)

        /* The first loop processes 48 bytes */
        addic. r5,r5,-48
        evldd r10,0(r4)
        evcmpeq 1,r6,r10
        evldd r7,8(r3)
        evldd r11,8(r4)

        /* Skip the loop, since there aren't enough left for the
         * loop to run once.  The epilog will take care of 16
         * bytes worth.
         */
        blt aligned_double_end

        /* This code is optimized to double issue every cycle, assuming
         * good branch prediction.  It does 4 compares per iteration, so
         * it processes 32 bytes in 10 cycles ==> 3.2 bytes/cycle.
         */
big_loop:
        evldd r8, 16(r3)
        bf 7,found_diff1
        evcmpeq 5,r7,r11
        evldd r12, 16(r4)
        bf 23,found_diff2
        evldd r9, 24(r3)
        addic. r5,r5,-32
        evldd r0, 24(r4)
        addi r3,r3,32
        addi r4,r4,32
        evcmpeq 6,r8,r12
        evldd r6,0(r3)
        bf 27,found_diff3
        evldd r10,0(r4)
        evcmpeq 7,r9,r0
        evldd r7,8(r3)
        bf 31,found_diff4
        evldd r11,8(r4)
        evcmpeq 1,r6,r10
        bge big_loop

        /* Finish off the epilog of the loop */
aligned_double_end:
        bf 7,found_diff1
        evcmpeq 5,r7,r11
        addi r3,r3,16
        addi r4,r4,16
        bf 23,found_diff2

        /* Redo these, because they get changed in the loop */
        mtcrf 0x2,r5
        mtcrf 0x1,r5
        b try_two_doubles

        /* Since we're here, we know that wordA and wordB are
         * different, but we need to return the result of
         * comparing the FIRST different word. We only return a
         * positive number if the first word is greater, or if
         * the first word is equal and the second word is
         * greater.  Otherwise we return a negative number.  The
         * boolean equation for greater would be gh + eh*gl.
         * After we compare for greaterness, we have gh, eh, and gl
         * (as well as some others).
         */
found_diff1:
        evcmpgtu 5,r6,r10
        li r9,1
        crand 6,21,4
        li r12,-1
        cror 6,6,20
        isel r3,r9,r12,6

        /* A new idea.  We only need to return a positive result if A is
         * greater than B and a negative if A < B.  A subtraction would
         * yield such a result.  The only thing we need to do is make
         * sure that we return the result word of the first DIFFERENT
         * words.  So we do an isel based on the equalNhi to select the
         * result
         *	evsubfw		r3, r10, r6
         *	evmergehi	r4, r3, r3
         *	isel		r3, r3, r4, 1
         */
        blr

found_diff2:
        evcmpgtu 1,r7,r11
        li r9,1
        crand 22,5,20
        li r12,-1
        cror 22,22,4
        isel r3,r9,r12,22
        blr

found_diff3:
        evcmpgtu 1,r8,r12
        li r9,1
        crand 26,5,24
        li r12,-1
        cror 26,26,4
        isel r3,r9,r12,26
        blr

found_diff4:
        evcmpgtu 1,r9,r0
        li r9,1
        crand 30,5,28
        li r12,-1
        cror 30,30,4
        isel r3,r9,r12,30
        blr

found_unaligned1:
        evcmpgtu 5,r6,r8
        li r9,1
        crand 6,21,4
        li r12,-1
        cror 6,6,20
        isel r3,r9,r12,6
        blr

found_unaligned2:
        evcmpgtu 1,r7,r9
        li r9,1
        crand 22,5,20
        li r12,-1
        cror 22,22,4
        isel r3,r9,r12,22
        blr

align_dest_word:
align_dest_double:
        /* First make sure there are at least 8 bytes left to
         * compare.  Otherwise, realignment could go out of bounds
         */
        cmpli cr0,r5,8
        neg r0,r3
        blt small_compare

        andi. r7,r3,0x3
        mtcrf 0x1,r0

        bne more_alignment

        /* Don't need to check if buffer A needs another word to be aligned.
         * We're here, therefore we must have only been off by a word.
         * So we shorten the path a bit by taking 2 branches out from the
         * more common path (ie things tend to be at least word-aligned)
         */
align_one_word:
        lwz r10,0(r4)
        addi r4,r4,4
        lwz r6,0(r3)
        addi r3,r3,4
        subfc. r0,r6,r10
        addi r5,r5,-4
        bne found_word_diff
        bne cr6,unaligned_double_compare
        cmpli cr1,r5,32
        mtcrf 0x1,r5
        b aligned_copy

more_alignment:
        bf 31, try_align_word
        lbz r10,0(r4)
        addi r4,r4,1
        lbz r6,0(r3)
        addi r3,r3,1
        subfc. r0,r6,r10
        addi r5,r5,-1
        bne found_word_diff

try_align_word:
        bf 30, try_align_double
        lhz r10,0(r4)
        addi r4,r4,2
        lhz r6,0(r3)
        addi r3,r3,2
        subfc. r0,r6,r10
        addi r5,r5,-2
        bne found_word_diff

try_align_double:
        bt 29, align_one_word
        cmpli cr1,r5,32
        mtcrf 0x1,r5
        beq cr6,aligned_copy

        /* For each double word copied, we load the double words with
         * each half from buffer B (r4), which starts at 0x*4 or 0x*c.  Then we
         * use evmergelohi to take the halves and rejoin them.  Notice
         * that any double load will necessarily be 4 bytes ahead.
         * Invariant: at the start of any block (except this first one)
         * which loads a doubleword, r10 will hold the first half of the
         * first doubleword
         */
unaligned_double_compare:
        /* align buffer B to a doubleword boundary */
        rlwinm r4,r4,0,0,28
        cmpli cr0,r5,16

        /* grab the first doubleword */
        evldd r11,0(r4)

        /* Set bits in the CR to indicate how many bytes will
         * remain after the big loop is done */
        mtcrf 0x2,r5
        mtcrf 0x1,r5
        bge unaligned_big_loop

        /* we need the old double to be in r10
         * for try_unaligned_double
         */
        evmr r10,r11

        /* There are less than 2 double words left, so we take care of
         * them.
         */
try_unaligned_double:
        bf 28, try_unaligned_word
        evldd r11,8(r4)
        addi r4,r4,8
        evldd r6,0(r3)
        addi r3,r3,8
        evmergelohi r10,r10,r11
        evcmpeq 1,r6,r10
        bf 7,found_diff1
        evmr r10,r11

try_unaligned_word:
        /* realign r4 to its original alignment, so
         * do_unaligned_last has the pointer in the right place
         */
        addi r4,r4,4
        cror 31,30,31
        bf 29, do_unaligned_last
        lwz r6,0(r3)
        subfc. r0,r6,r10
        addi r4,r4,4
        addi r3,r3,4
        bne found_word_diff

do_unaligned_last:
        bf 31,unaligned_end
        lwz r6,0(r3)
        rlwinm r5,r5,3,27,28
        lwz r10,0(r4)
        li r8,-1
        srw r8,r8,r5
        or r6,r6,r8
        or r10,r10,r8
        subfc. r0,r6,r10
        bne found_word_diff

unaligned_end:
        li r3,0
        blr

unaligned_big_loop:
        /* At this point, we know there are at least 16 bytes of
         * data to compare, so start loading them.
         * Also, the loop updates at the end, so we need to
         * update here to keep up.
         */
        evldd r10,8(r4)
        addic. r5,r5,-24
        evldd r6,0(r3)
        addi r4,r4,8
        addi r3,r3,8
        blt unaligned_big_loop_end

unaligned_big_loop_really:
        evmergelohi r8,r11,r10
        evldd r11,8(r4)
        evcmpeq 1,r6,r8
        evldd r7,0(r3)
        addic. r5,r5,-16
        bf 7,found_unaligned1
        evmergelohi r9,r10,r11
        evldd r10,16(r4)
        evcmpeq 5,r7,r9
        evldd r6,8(r3)
        addi r4,r4,16
        bf 23,found_unaligned2
        addi r3,r3,16
        bge unaligned_big_loop_really

unaligned_big_loop_end:
        evmergelohi r8,r11,r10
        evcmpeq 1,r8,r6
        mtcrf 0x1,r5
        bf 7,found_unaligned1
        b try_unaligned_double


small_compare:
        mtcrf 0x1,r5
        bf 29,try_small_half
        lbz r6,0(r3)
        lbz r10,0(r4)
        subfc. r0,r6,r10
        bne found_word_diff
        lbz r7,1(r3)
        lbz r11,1(r4)
        subfc. r0,r7,r11
        bne found_word_diff
        lbz r8,2(r3)
        lbz r12,2(r4)
        subfc. r0,r8,r12
        bne found_word_diff
        lbz r9,3(r3)
        lbz r0,3(r4)
        subfc. r0,r9,r0
        bne found_word_diff
        addi r3,r3,4
        addi r4,r4,4

try_small_half:
        bf 30,try_small_byte
        lbz r6,0(r3)
        lbz r10,0(r4)
        subfc. r0,r6,r10
        bne found_word_diff
        lbz r7,1(r3)
        lbz r11,1(r4)
        subfc. r0,r7,r11
        bne found_word_diff
        addi r3,r3,2
        addi r4,r4,2

try_small_byte:
        bf 31, small_finish
        lbz r6,0(r3)
        lbz r10,0(r4)
        subfc. r0,r6,r10
        bne found_word_diff

small_finish:
        li r3,0
        blr

memcmp_unaligned:
        /* If both pointers can be double-aligned, align r3,
         * setting eq6 to indicate "aligned double"
         */
        rlwinm. r0,r0,0,29,31 /* clrlwi 29 */

        /* Do this instead of crset because unlike crset, it
         * isn't serialized.  Using a nonvolatile register for
         * comparison because we know it hasn't been used.
         */
        cmpw cr6,r31,r31 /* set eq6 */

        /* Look at r3 to see if we're aligned already (but not
         * both aligned, which is why we're here)
         */
        rlwinm r7,r3,0,29,31
        beq align_dest_double

        /* If both pointers can be word-aligned, align dest,
         * clearing eq6 to indicate unaligned.
         * Compare to find out if dest is already doublealigned
         */
        rlwinm. r0,r0,0,30,31
        cmpwi cr1,r7,0

        /* Only skip to unaligned_double_compare if r3 is aligned,
         * and r0 indicates word-alignment
         */
        crand 6,6,2

        crxor 26,26,26 /* clear eq6 */
        beq cr1,unaligned_double_compare
        beq align_dest_word

        /* Before we hop into bytewise comparing, make sure that
         * there are bytes to compare (don't want to loop 4 billion+
         * times!
         */
        cmpwi r5,0
        beq byte_done

        /* Well, alignment is just icky, compare bytewise */
        mtctr r5
byte_compare:
        lbz r6,0(r3)
        addi r3,r3,1
        lbz r10,0(r4)
        addi r4,r4,1
        subfc. r0,r6,r10
        bne found_word_diff
        bdnz byte_compare

byte_done:
        li r3,0
        blr

        .size FUNCTION, .-FUNCTION
