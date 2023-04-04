/*------------------------------------------------------------------
 * strcmp.S
 *
 * Standard strcmp function optimized for e500 using SPE
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
 * int strcmp(const unsigned char* sourceA,
 *            const unsigned char* sourceB);
 * Returns:
 *  value < 0  if source1 < source2
 *  value = 0  if source1 = source2
 *  value > 0  if source1 > source2
 *------------------------------------------------------------------
 */

	.file   "strcmp.S"
	.section        ".text"
	.align 4
#define FUNCTION __strcmp_e500
	.globl FUNCTION
	.type FUNCTION,@function
/* If sourceA and sourceB are aligned the same wrt doublewords,
 * or aligned the same wrt words, we compare in doublewords.
 * Otherwise, go byte by byte.
 */
FUNCTION:

	/* Put the address of the constants in r11 */
        addis r11,0,strange_constants@ha
        addi r11,r11, strange_constants@l

        /* r3 will be used for returning stuff, so lets move
	 * sourceA out of there
	 */
        mr r10,r3

        /* Load the 0x01010101 constant */
        evldd r12,0(r11)

        /* generate a mask to set the unwanted bits to 1
	 * take the alignment of r10 * 8
	 */
        rlwinm r6, r10, 3, 27,28

        /* Load the 0x80808080 constant */
        evldd r11,8(r11)

        /* start the mask with all Fs */
        li r8, -1

        or r0,r10,r4

        subfic r6, r6, 32

        andi. r0,r0,0x7

        /* Shift r8 by r6 to mask off preceding bytes */
        slw r8, r8, r6

        /* Save the stack, cuz we need to save a reg */
        stwu r1, -32(r1)

        /* save r14 for counting.  This is cheaper than bdnz */
        stw r14, 8(r1)

        beq Aligned_Double

/* load the first words (on word boundaries) and process them,
 * setting bits preceding the first byte to 1.
 */

        /* If r10 and r4 can't be aligned to words, then jump
	 * to unaligned compare
	 */
        xor r0,r10,r4
        rlwinm. r0,r0,0,30,31
        bne unaligned_compare

        /* align r10 to word */
        rlwinm r10, r10, 0,0,29

        /* process the first word, and word align r4 */
        lwz r5, 0(r10)
        rlwinm r4, r4, 0,0,29

        /* Check whether r10 and r4 are both aligned double or not.
	 * Jump to Unaligned_Double if they are different.
	 */
        xor r0,r10,r4
        addi r10, r10, 4

        /* Load the other first word (B), and set all the bits
	 * preceding the first real byte
	 */
        lwz r7, 0(r4)
        or r5, r5, r8

        /* This will be 0b100 if the ptrs are only word aligned
	 * (not doubleword aligned)
	 */
        rlwinm r0,r0,0,29,29

        /* Look for nulls in the first (partial) word */
        subfc r3, r12, r5
        andc r9, r11, r5

        /* Update r4, and find out if there were any nulls */
        addi r4,r4,4
        and. r9, r3, r9

        /* Find out if we are doubleword aligned, and set all the
	 * bits preceding the first byte in word B
	 */
        cmpwi 6, r0,0
        or r7,r7,r8

        /* jump out if we found a null, otherwise compare the two words */
        bne find_null1
        subfc. r0, r5, r7

        /* Set a condition bit if r10 is not yet doubleword
	 * aligned, and branch if the first words were different
	 */
        mtcrf 0x01,r10
        bne found_diff_early

        /* process another word if necessary to align to a doubleword
	 * boundary.
	 */
        bf 29, Check_Aligned_Double

        /* Load another word from each string, to align r10 to a
	 * double word
	 */
        lwz r5, 0(r10)
        addi r10, r10, 4
        lwz r7, 0(r4)
        addi r4,r4,4


        /* Check for nulls and differences in the words */
        subfc r3, r12, r5
        andc r9, r11, r5
        and. r9, r3, r9
        bne find_null1
        subfc. r0, r5, r7
        bne found_diff_early

        /* r10 is now aligned to a doubleword boundary, but it
	 * may be that we are not both double aligned.  We set a
	 * bit based on this earlier, so branch if it is set.
	 */
Check_Aligned_Double:
        bne 6, Unaligned_Double

        /* loop through 2 doublewords at once in this page, until one of
	 * these happens:
	 * 1) A null byte is found in r10
	 * 2) a byte is found which is different between r10 and r4
	 * 3) the end of the page is reached
	 *
	 * If 3 happens, then we finish processing the last doubleword in
	 * the page, and if it checks out, we jump to the next one.  The
	 * hope is that no small strings are going to cross page
	 * boundaries.  For large strings, the hit should be minimal
	 *
	 * If 1 happens, some extra checking goes on to see whether the
	 * strings are the same or not.
	 */
Aligned_Double:
        /* Start figuring out how far to the next page */
        not r6,r10
        not r8,r4

        rlwinm r6,r6,29,23,31
        rlwinm r8,r8,29,23,31

        subfc. r0,r6,r8
        evldd r5, 0(r10)

        /* zero out a reg for comparison (the result of the final
	 * and is nonzero for any word with a null byte)
	 */
        evsplati r0,0
        evldd r7, 0(r4)

        /* Select the shortest distance to the page */
        isellt r14,r8,r6

        evsubfw r3, r12, r5
        addic. r14,r14,-6

        evandc r9, r11, r5

        blt aligned_double_loop_end

        /* The loop */
aligned_double_loop:
        evldd r6, 8(r10)
        evand r9, r9, r3
        addi r10,r10,16
        evcmpgtu 1, r9, r0
        evldd r8, 8(r4)
        evcmpeq 6, r5, r7
        bt 6, found_null1
        evsubfw r3,r12, r6
        bf 27, found_diff1
        evandc r9, r11, r6
        evldd r5, 0(r10)
        evand r9,r9,r3
        addi r4,r4,16
        evcmpgtu 5, r9, r0
        evldd r7, 0(r4)
        evcmpeq 7, r6, r8
        bt 22, found_null2
        evsubfw r3, r12, r5
        bf 31, found_diff2
        evandc r9, r11, r5

        evldd r6, 8(r10)
        evand r9, r9, r3
        addi r10,r10,16
        evcmpgtu 1, r9, r0
        evldd r8, 8(r4)
        evcmpeq 6, r5, r7
        bt 6, found_null1
        evsubfw r3,r12, r6
        bf 27, found_diff1
        evandc r9, r11, r6
        evldd r5, 0(r10)
        evand r9,r9,r3
        addi r4,r4,16
        evcmpgtu 5, r9, r0
        evldd r7, 0(r4)
        evcmpeq 7, r6, r8
        bt 22, found_null2
        evsubfw r3, r12, r5
        bf 31, found_diff2
        addic. r14,r14,-4
        evandc r9, r11, r5
        bge aligned_double_loop

        /* we exited the loop, we must be at the end of a page.
	 * Finish the last one, and then see if we are done or
	 * not.
	 */
aligned_double_loop_end:
        evand r9, r9, r3
        evcmpgtu 1, r9, r0
        evcmpeq 6, r5, r7
        bt 6, found_null1
        bf 27, found_diff1
        addi r10,r10,8
        addi r4,r4,8

        or r3, r10,r4
        andi. r3, r3, 0xFFF
        beq find_next_page
        /* We need to check the next doubleword, too.  It may be
	 * in the next page, but it may be the last one in this
	 * page.
	 */
        li r14,3
find_page_end:
        evldd r5, 0(r10)
        addi r10,r10,8
        evldd r7, 0(r4)
        addi r4,r4,8
        evsubfw r3, r12, r5
        addic. r14,r14,-1
        evandc r9, r11, r5
        evand r9, r9, r3
        evcmpgtu 1, r9, r0
        evcmpeq 6, r5, r7
        bt 6, found_null1
        bf 27, found_diff1
        bne find_page_end


        /* It is now safe to proceed to the next page.  load the
	 * counter with the number of doublewords before the next
	 * page
	 */

find_next_page:
        not r6,r10
        not r8,r4
        rlwinm r6,r6,29,23,31
        rlwinm r8,r8,29,23,31
        subfc. r0,r6,r8
        isellt r14,r8,r6
        evldd r5, 0(r10)
        addic. r14,r14,-6
        evldd r7, 0(r4)
        evsubfw r3, r12, r5
        evandc r9, r11, r5
        evsplati r0, 0
        blt aligned_double_loop_end
        b aligned_double_loop

        /* The first doubleword had a null byte */
found_unaligned_null1:
        evmr r7,r15
        evldd r15, 16(r1)
found_null1:
        /* If there was a null in the hi word, or if the hi words
	 * were not equal, we want to check the hi word, so move
	 * it down into the lo word
	 */
        crorc 4,4,24
        bf 4, find_null1
        evmergehi r5, r5, r5
        evmergehi r7,r7,r7
find_null1:
        rlwinm. r11,r5,0,0,7
        li r12,24
        beq aligned_shift1
        rlwinm. r11,r5,0,8,15
        li r12,16
        beq aligned_shift1
        rlwinm. r11,r5,0,16,23
        li r12,8
        beq aligned_shift1
        li r12,0

        /* If the signs are different for these words,
	 * then overflow can occur
	 */
        xor. r0,r5,r7

aligned_shift1:
        srw r5,r5,r12
        srw r7,r7,r12
        lwz r14, 8(r1)
        subfc r3,r7,r5
        addi r1,r1,32

        /* return now if the signs were the same (r3
	 * has the appropriate value).  Otherwise, return
	 * r7.  r7's sign matches that of the necessary
	 * return value, since the comparison is actually
	 * unsigned.  This means that a negative number is
	 * actually large, and, if the signs are different,
	 * guaranteed to be larger than r5. The same is true
	 * for the opposite case.  We also make sure it is
	 * non-zero.
	 */
        bgelr
        ori r3,r7,1
        blr

found_unaligned_null2:
        evmr r8,r15
        evldd r15, 16(r1)
found_null2:
        /* If there was a null in the hi word, move that word
	 * down into the lo word
	 */
        evmr r5,r6
        crorc 20, 20, 28
        evmr r7,r8
        bf 20, find_null1
        evmergehi r5, r5, r5
        evmergehi r7, r7, r7
        b find_null1

/* Now we know that there is a difference in one of the
   two doublewords.  We find the first different bit within
   each word, and then rotate each word of B so that the sign
   bit is that bit.  When B is greater, that bit will be one, and
   the sign of the return should be negative, and when B is less,
   the bit will be zero, making the return positive.  Also,
   we need to OR the r3 with 1 to make sure it is non-zero
 */
found_unaligned1:
        evmr r7,r15
        evldd r15, 16(r1)
found_diff1:
        evxor r0,r5,r7
        lwz r14,8(r1)
        evcntlzw r0,r0
        addi r1,r1,32
        evrlw r3,r7,r0
        evmergehi r4,r3,r3
        isel r3,r3,r4,24
        ori r3,r3,1
        blr

/* See found_diff1 description for explanation of this code */
found_unaligned2:
        evmr r8,r15
        evldd r15, 16(r1)
found_diff2:
        evxor r0,r6,r8
        lwz r14,8(r1)
        evcntlzw r0,r0
        addi r1,r1,32
        evrlw r3,r8,r0
        evmergehi r4,r3,r3
        isel r3,r3,r4,28
        ori r3,r3,1
        blr

Unaligned_Double:
        /* Invert the pointers so that the last 12 bits hold the
	 * number of bytes to the end of the page (minus 1)
	 */
        not r6,r10
        not r8,r4

        /* remove all but the last 12 bits from the nots, and
	 * shift the result right by 3, thus making it the number
	 * of doubles to the end of the page
	 */
        rlwinm r6,r6,29,23,31
        rlwinm r8,r8,29,23,31

        /* align r4 to a doubleword boundary, and load the
	 * first double of r10 (prologue for loop)
	 */
        rlwinm r4,r4,0,0,28
        evldd r5, 0(r10)

        /* subtract the distances from each other, setting the
	 * condition code so we know which was shorter.
	 * And load the first word of r4 into a doubleword
	 */
        subfc. r0,r6,r8
        evldd r7, 0(r4)

        /* zero out a reg for comparison (the result of the final
	 * and is nonzero for any word with a null byte)
	 * Save off a register for merging into
	 */
        evsplati r0,0
        evstdd r15,16(r1)

        /* And start looking for nulls in r10 */
        evsubfw r3, r12, r5

        /* Select the shortest distance to the page */
        isellt r14,r8,r6

        /* And continue looking for nulls */
        evandc r9, r11, r5

        /* We want to jump out early (before we load the next
	 * double) if this is the last double.
	 * Then update r14 so that it reflects the number of
	 * doublewords that will have been loaded by the end
	 * of the first iteration of the loop (4)
	 */
        cmpwi cr1,r14,1

        addic. r14,r14,-4

        /* Can't do this load until we know the previous one is
	 * free of null bytes
	 */
        blt cr1,unaligned_last_was_first
        evldd r8, 8(r4)

        /* Align the first double,  and branch over the loop if
	 * we are less than 3 doubles from the end of the page
	 */
        evmergelohi r15,r7,r8
        blt unaligned_double_loop_end

        /* And away we go!  Loop as many times as we can before
	 * we hit the end of the page.  This code is scheduled
	 * such that there are no stalls in execution.  Because
	 * loads take 3 cycles, there could be stalls in
	 * completion.
	 * NOTE TO SELF:
	 *
	 * Issues:
	 * 1) evmergelohi must be before any connected branch.
	 *   the exit cases (found_null1, etc) assume that the
	 *   values are all properly lined up.
	 * 2) null case must branch before diff case.  diff
	 *    assumes it to be so
	 * 3) When near the end of the page, have to make sure
	 *    that the hi word of r10, and the lo word of r4
	 *    don't have any nulls before grabbing the next double
	 *    for r4
	 */
unaligned_loop:
        evcmpeq 6, r5,r15
        evldd r6, 8(r10)
        evand r9,r9,r3
        addi r10,r10,16
        evcmpgtu 1,r9, r0
        evldd r7, 16(r4)
        evsubfw r3,r12,r6
        bt 6,found_unaligned_null1
        evandc r9,r11,r6
        bf 27,found_unaligned1
        evmergelohi r15,r8,r7
        addi r4,r4,16
        evcmpeq 7,r6,r15
        evldd r5,0(r10)
        evand r9,r9,r3
        addic. r14,r14,-2
        evcmpgtu 5,r9, r0
        evldd r8,8(r4)
        evsubfw r3,r12,r5
        bt 22,found_unaligned_null2
        evandc r9,r11,r5
        bf 31,found_unaligned2
        evmergelohi r15,r7,r8
        bge unaligned_loop

        /* we exited the loop, we must be at the end of a page.
	 * Finish the last one, and then see if we are done or
	 * not.
	 */
unaligned_double_loop_end:
        evcmpeq 6, r5, r15
        evand r9,r9,r3
        evcmpgtu 1, r9, r0
        addi r10,r10,8
        bt 6, found_unaligned_null1
        addi r4,r4,8
        bf 27, found_unaligned1

        /* We need to check the next doubleword, too.  It may be
	 * in the next page, but it may be the last one in this
	 * page.
	 */
unaligned_find_page_end:
        evldd r6, 0(r10)
        addi r10,r10,8
        evsubfw r3, r12, r6
        evandc r9, r11, r6
        evand r9, r9, r3
        evcmpgtu 5, r9, r0
        bt 20,found_early_null2

        /* Check r8 */
        evsubfw r3, r12,r8
        evandc r9, r11,r8
        evand r9,r9,r3
        evcmpgtu 1,r9,r0
        bt 6,found_early_null2
        evldd r7, 8(r4)
        addi r4,r4,8
        evmergelohi r15,r8,r7
        evcmpeq 7, r6, r15
        bt 22, found_unaligned_null2
        bf 31, found_unaligned2


        /* At this point, we will have crossed a page boundary in
	 * one of the strings safely.  However, the other string
	 * could be near the end of the page.  So we calculate
	 * which string is closest to the end of the page again,
	 * and redo the end of page code if it is less than 3
	 * doublewords from the end.
	 */
unaligned_find_next_page:
        not r6,r10
        not r8,r4
        rlwinm r6,r6,29,23,31
        rlwinm r8,r8,29,23,31
        subfc. r0,r6,r8
        evldd r5, 0(r10)
        evsplati r0,0
        isellt r14,r8,r6
        cmpwi cr1,r14,1
        evsubfw r3, r12, r5
        addic. r14,r14,-3
        evandc r9, r11, r5
        blt cr1,unaligned_last_was_first
        evldd r8, 8(r4)
        evmergelohi r15,r7,r8
        blt unaligned_double_loop_end
        b unaligned_loop

/* The end of the page happened so early, we couldn't load more
 * than one doubleword safely.
 * We need to make sure both CURRENTLY LOADED doublewords are
 * null-free, since either one could be the one at the end of the
 * page.
 */
unaligned_last_was_first:
        evand r9,r9,r3
        addi r10,r10,8
        evcmpgtu 1,r9,r0
        bt 4,found_early_null

        /* now check r7 */
        evsubfw r3,r12,r7
        evandc r9,r11,r7
        evand r9,r3,r9
        evcmpgtu 5,r9,r0
        bt 22,found_early_null

        /* Now load the next 2 words of r4, and realign the
	 * data to r10.  Then branch out if any of the bytes
	 * were null.
	 */
        evldd r8,8(r4)
        addi r4,r4,8
        evmergelohi r15,r7,r8
        evcmpeq 6,r5,r15
        bt 6,found_unaligned_null1
        bf 27, found_unaligned1

        /* unaligned_find_next_page expects the previous
	 * doubleword to be in r7, so move r8 there
	 */
        evmr r7,r8

        /* find out where the next page is */
        b unaligned_find_next_page

/* We found a null in the hi word of r5, near the end of the
 * page.  r7 has the value we want to deal with, but we want
 * the important word in A to be in the lo word.  So we merge
 */
found_early_null:
        evmergehi r5,r5,r5
        evldd r15,16(r1)
        b find_null1

found_early_null2:
        evmergehi r5,r6,r6
        evldd r15,16(r1)
        evmr r7,r8
        b find_null1

unaligned_compare:
        lbz r5, 0(r10)
        lbz r7, 0(r4)
        lwz r14, 8(r1)
        addi r1,r1,32
        subfc. r3, r7, r5
        bnelr
unaligned_byte_loop:
        cmpwi cr1, r5,0
        beq cr1, Null_Byte
        lbzu r5,1(r10)
        lbzu r7,1(r4)
        subfc. r3,r7,r5
        beq unaligned_byte_loop
        blr
Null_Byte:
        li r3,0
        blr

found_diff_early:
        subfe r3,r0,r0
        not r3,r3
        ori r3,r3,1
        lwz r14, 8(r1)
        addi r1,r1,32
        blr

        .size FUNCTION, .-FUNCTION

/* Data section */
        .sect ".data", "aw"
        .align 4
        .type strange_constants, @object
        .size strange_constants, 16
strange_constants:
        .long 0x01010101
        .long 0x01010101
        .long 0x80808080
        .long 0x80808080
