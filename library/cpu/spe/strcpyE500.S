/*------------------------------------------------------------------
 * strcpy.S
 *
 * Standard strcpy function optimized for e500 using SPE
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
 *------------------------------------------------------------------
 */

/*------------------------------------------------------------------
 * char * strcpy(char* dest, const char* src);
 * Returns:
 * dest
 *------------------------------------------------------------------
 */

	.file   "strcpy.S"
	.section        ".text"
	.align 4
#define FUNCTION __strcpy_e500
	.globl FUNCTION
	.type FUNCTION,@function
FUNCTION:
/* If dest and src are aligned the same wrt doublewords,
 * or aligned the same wrt words, we copy in doublewords.
 * Otherwise, go byte by byte.
 */

        /* All of the string functions use an algorithm for null
	 * detection taken from the original libmoto:
	 *
	 * 1) load in a word
	 * 2a) subtract 0x01010101 from the word.  This will cause
	 * all bytes with a null to be 0xff (unless it is right
	 * before another null, in which case it will be 0xfe)
	 *
	 * 3b) AND 0x80808080 with the complement of the word.
	 * The resulting bytes will only have their high bit set
	 * if the corresponding bit was 0 in the original word
	 *
	 * So now we (basically) have a word with the high bit of
	 * any byte set which was between 0x81 and 0xff, or 0x00.
	 * We also have a mask which will only select the high
	 * bits of bytes which were between 0x00 and 0x7f.  There
	 * is only one overlap:  a null byte (0x00).  If we now:
	 *
	 * 4) and the two results
	 *
	 * The result will only have 1s in the high bits of bytes
	 * which had nulls.  With one exception.  If there is a
	 * byte with 0x01 before a byte with 0x00, then that
	 * byte, too, will have a 1 in the high bit.  However,
	 * since this only happens when a byte with 0x00 occurs
	 * in the word, this technique is useful for detecting if
	 * there are any nulls in the word.
	 */

        /* r3 needs to return the original dest, but r3 is used,
	 * so move r3 into the upper half of r10
	 */
        evmergelo r10,r3,r3

        /* Put the address of the constants in
	 * r11 (we'll overwrite it when we load 0x8080...)
	 */
        lis r11,fun_constants@ha
        addi r11,r11, fun_constants@l

        /* Load the 0x01010101 constant */
        evldd r12,0(r11)

        /* Determine whether r4 and r10 are doubleword-aligned */
        or r0,r10,r4

        /* Load the 0x80808080 constant */
        evldd r11,8(r11)

        /* Save the stack, cuz we need to save a reg */
        stwu r1, -16(r1)

        andi. r0,r0,0x7

        /* save r14.  This is cheaper than bdnz */
        stw r14, 8(r1)

        beq Aligned_Double

/* load the first words (on word boundaries) and process them,
 * setting bits preceding the first byte to 1.
 */

        /* If r10 and r4 are off by 4, we can handle that almost
	 * as fast as in the fully-aligned case, so we want to go
	 * there as soon as possible.  If this is true, then
	 * r0 will be 4.  The Unaligned_Double loop needs
	 * r10 to be double-aligned, so we move bit 29 of r10
	 * into bit 29 of r0, and if r0 is then zero,
	 * r10 is already properly aligned, and we jump to
	 * Unaligned_Double.
	 */
        rlwimi. r0,r10,0,29,29
        not r6, r10
        xor r0,r10,r4
        beq Unaligned_Double

        /* If the last two bits of r4 and r10 are
	 * different, then it is not efficient to copy
	 * them in chunks larger than a byte
	 */
        andi. r14,r0,0x3
        addi r6, r6, 1
        bne unaligned_copy

        /* Check for double alignment */
        rlwinm r0,r0,0,29,29

        /* Move the alignment amount into the CR */
        mtcrf 0x1, r6

        /* Set the condition code for double alignment */
        cmpwi 5, r0,0

        /* Process a byte if r10 is only byte-aligned */
        bf 31, try_halfword
        lbz r5, 0(r4)
        addi r4,r4,1
        cmpwi r5, 0
        stb r5, 0(r10)
        addi r10,r10,1
        beq aligned_end1

        /* Process 2 bytes if r10 is only halfword aligned */
try_halfword:
        bf 30, try_word
        lbz r5, 0(r4)
        lbz r6, 1(r4)
        addi r4,r4,2
        cmpwi r5, 0
        stb r5, 0(r10)
        beq aligned_end1
        cmpwi r6, 0
        stb r6, 1(r10)
        beq aligned_end1
        addi r10,r10,2

        /* process another word if necessary to align to a doubleword
	 * boundary.
	 */
try_word:
        bf 29, Check_Aligned_Double

        /* Load the next word, to align r10 to a double word */
        lwz r5, 0(r4)
        addi r4,r4,4

        /* Process it */
        subfc r3, r12, r5
        andc r9, r11, r5
        and. r9, r3, r9

        /* Need to have r5 in r0, because find_null1 assumes
	 * that r0 will have the word in which the first null was
	 * found.  (found_null1 assumes that condition codes have
	 * been set by evcmpgtu, so we have to jump into that
	 * block of code after the logic which figures out the
	 * first word has been executed)
	 */
        mr r0,r5
        bne find_null1

        /* Store the word */
        stw r5,0(r10)
        addi r10, r10, 4

        /* r10 is now doubleword aligned, but both strings may
	 * not be doubleword aligned.  We set a bit earlier for
	 * this, so branch if it was set.
	 */
Check_Aligned_Double:
        bne 5, Unaligned_Double

        /* loop through 2 doublewords at once in this page, until one of
	 * these happens:
	 * 1) A null byte is found in r4
	 * 2) the end of the page is reached
	 *
	 * If 2 happens, then we finish processing the last doubleword in
	 * the page, and if it checks out, we jump to the next one.  The
	 * hope is that no small strings are going to cross page
	 * boundaries.  For large strings, the hit should be minimal
	 *
	 * If 1 happens, some extra checking goes on to see whether the
	 * strings are the same or not.
	 *
	 * The primary loop is capable of 2 IPC (it issues
	 * 2/cycle, but completion may stall).  If it gets 2 IPC,
	 * then it is doing a load or store every other cycle.
	 * This means that a doubleword is stored every fourth
	 * cycle.  So the code will approach copying 2
	 * bytes/cycle.  This is 2x what the optimized scalar
	 * code can do.
	 */
Aligned_Double:
        /* Start figuring out how far to the next page */
        not r8,r4

        evldd r5, 0(r4)

        /* zero out a reg for comparison (the result of the final
	 * and is nonzero for any word with a null byte)
	 */
        evsplati r0,0

        rlwinm r8,r8,29,23,31

        evsubfw r3, r12, r5

        /* We will load 2 doublewords, but it could be that the
	 * first null is in the the first double.
	 * If this double is also the last in the page, then
	 * loading the next double could cause a segmentation
	 * fault.  So we check to see if this is the last, and if
	 * so we jump to a special case.
	 */
        addic. r14, r8,-1
        blt last_double_was_first

        evldd r6, 8(r4)
        evandc r9, r11, r5

        evand r9,r9,r3
        addic. r14,r14,-5

        evcmpgtu 1,r9,r0
        evsubfw r3,r12, r6

        blt aligned_double_loop_end
        /* The loop */
aligned_double_loop:
        evandc r9, r11, r6
        evldd r7, 16(r4)
        evand r9,r9,r3
        bt 6, found_null1
        evcmpgtu 5, r9, r0
        evstdd r5,0(r10)
        evsubfw r3, r12, r7
        evldd r8, 24(r4)
        evandc r9, r11, r7
        bt 22, found_null2
        evand r9, r9, r3
        evstdd r6,8(r10)
        evcmpgtu 6, r9, r0
        addic. r14,r14,-4
        evsubfw r3,r12, r8
        addi r4,r4,32
        evandc r9, r11, r8
        evldd r5, 0(r4)
        evand r9, r9, r3
        bt 26, found_null3
        evcmpgtu 7, r9, r0
        evstdd r7,16(r10)
        evsubfw r3,r12, r5
        evldd r6, 8(r4)
        evandc r9, r11, r5
        bt 30, found_null4
        evand r9, r9, r3
        evstdd r8,24(r10)
        evcmpgtu 1, r9, r0
        addi r10,r10,32
        evsubfw r3,r12, r6
        bge aligned_double_loop

        /* we exited the loop, we must be at the end of a page.
	 * Finish the last one, and then see if we are done or
	 * not.
	 */
aligned_double_loop_end:
        bt 6, found_null1
        evstdd r5, 0(r10)
        evandc r9,r11,r6
        evand r9,r9,r3
        evcmpgtu 5,r9,r0
        bt 22, found_null2
        evstdd r6, 8(r10)

        addi r4,r4,16
        addi r10,r10,16

        andi. r3,r4,0xFFF
        beq start_new_loop

find_page_end:
        evldd r5,0(r4)
        addi r4,r4,8
        evsubfw r3, r12, r5
        evandc r9, r11, r5
        evand r9, r3, r9
        andi. r3,r4,0xFFF
        evcmpgtu 1, r9, r0
        bt 6, found_null1
        evstdd r5, 0(r10)
        addi r10,r10,8
        bne find_page_end

start_new_loop:
        /* It is now safe to proceed to the next page.  load the
	 * r14 with the number of doublewords in a page
	 * (-6 to account for the first loop)
	 */
        evldd r5, 0(r4)
        li r14, 505

        /* Now we set up for the beginning of the loop */
        evldd r6, 8(r4)
        evsubfw r3, r12, r5
        evandc r9, r11, r5
        evand r9,r9,r3
        evcmpgtu 1,r9,r0
        evsubfw r3,r12,r6
        b aligned_double_loop

last_double_was_first:
        addi r4,r4,8
        evandc r9,r11,r5
        evand r9,r9,r3
        evcmpgtu 1,r9,r0
        bt 6,found_null1
        evstdd r5,0(r10)
        addi r10,r10,8
        b start_new_loop


        /* The first doubleword had a null byte */
found_null1:
        /* If there was a null in the hi word, move that word
	 * down into the lo word
	 */
        evmergehi r0, r5, r5
        bt 4, find_null1
        stw r0,0(r10)
        addi r10,r10,4
        mr r0,r5
find_null1:
        rlwinm. r11,r0,8,24,31
        stb r11, 0(r10)
        beq aligned_end1
        rlwinm. r11,r0,16,24,31
        stb r11, 1(r10)
        beq aligned_end1
        rlwinm. r11,r0,24,24,31
        stb r11, 2(r10)
        beq aligned_end1
        stb r0, 3(r10)

aligned_end1:
        evmergehi r3,r10,r10
        lwz r14, 8(r1)
        addi r1,r1,16
        blr

        /* The second doubleword had a null byte */
found_null2:
        /* If there was a null in the hi word, move that word
	 * down into the lo word
	 */
        evmergehi r0, r6, r6
        bt 20, find_null2
        stw r0,8(r10)
        addi r10,r10,4
        mr r0,r6
find_null2:
        rlwinm. r11,r0,8,24,31
        stb r11, 8(r10)
        beq aligned_end2
        rlwinm. r11,r0,16,24,31
        stb r11, 9(r10)
        beq aligned_end2
        rlwinm. r11,r0,24,24,31
        stb r11, 10(r10)
        beq aligned_end2
        stb r0, 11(r10)

aligned_end2:
        evmergehi r3,r10,r10
        lwz r14, 8(r1)
        addi r1,r1,16
        blr

        /* The third doubleword had a null byte */
found_null3:
        /* If there was a null in the hi word, move that word
	 * down into the lo word
	 */
        evmergehi r0, r7, r7
        bt 24, find_null3
        stw r0,16(r10)
        addi r10,r10,4
        mr r0,r7
find_null3:
        rlwinm. r11,r0,8,24,31
        stb r11,16(r10)
        beq aligned_shift3
        rlwinm. r11,r0,16,24,31
        stb r11,17(r10)
        beq aligned_shift3
        rlwinm. r11,r0,24,24,31
        stb r11,18(r10)
        beq aligned_shift3
        stb r0,19(r10)

aligned_shift3:
        evmergehi r3,r10,r10
        lwz r14, 8(r1)
        addi r1,r1,16
        blr


        /* The fourth doubleword had a null byte */
found_null4:
        /* If there was a null in the hi word, move that word
	 * down into the lo word
	 */
        evmergehi r0, r8, r8
        bt 28, find_null4
        stw r0,24(r10)
        addi r10,r10,4
        mr r0,r8
find_null4:
        rlwinm. r11,r0,8,24,31
        stb r11,24(r10)
        beq aligned_shift4
        rlwinm. r11,r0,16,24,31
        stb r11,25(r10)
        beq aligned_shift4
        rlwinm. r11,r0,24,24,31
        stb r11,26(r10)
        beq aligned_shift4
        stb r0,27(r10)

aligned_shift4:
        evmergehi r3,r10,r10
        lwz r14, 8(r1)
        addi r1,r1,16
        blr

found_unaligned_null1:
        evmergehi r0,r7,r7
        bt 24,find_unaligned_null1
        stw r0,0(r10)
        addi r10,r10,4
        mr r0,r7
find_unaligned_null1:
        rlwinm. r11,r0,8,24,31
        stb r11, 0(r10)
        beq unaligned_end1
        rlwinm. r11,r0,16,24,31
        stb r11, 1(r10)
        beq unaligned_end1
        rlwinm. r11,r0,24,24,31
        stb r11, 2(r10)
        beq unaligned_end1
        stb r0, 3(r10)

unaligned_end1:
        evmergehi r3,r10,r10
        lwz r14, 8(r1)
        addi r1,r1,16
        blr

found_unaligned_null2:
        evmergehi r0,r8,r8
        bt 28,find_unaligned_null2
        stw r0,8(r10)
        addi r10,r10,4
        mr r0,r8
find_unaligned_null2:
        rlwinm. r11,r0,8,24,31
        stb r11, 8(r10)
        beq unaligned_end2
        rlwinm. r11,r0,16,24,31
        stb r11, 9(r10)
        beq unaligned_end2
        rlwinm. r11,r0,24,24,31
        stb r11, 10(r10)
        beq unaligned_end2
        stb r0, 11(r10)

unaligned_end2:
        evmergehi r3,r10,r10
        lwz r14, 8(r1)
        addi r1,r1,16
        blr

Unaligned_Double:
        /* Align r4 to doubleword.
	 * Get number of bytes before end of page
	 */
        not r8,r4
        rlwinm r4,r4,0,0,28

        /* Load the first doubleword (only half of which is good) */
        rlwinm r8,r8,29,23,31
        evldd r6,0(r4)

        /* We will load 2 doublewords, but it could be that the
	 * first null is in the lower word of the first double.
	 * If this double is also the last in the page, then
	 * loading the next double could cause a segmentation
	 * fault.  So we check to see if the word has any
	 * nulls, and if so, we jump to a special case
	 * And zero a register for comparison
	 */
        addic. r14,r8,-1
        evsplati r0,0
        addi r10,r10,-16 /* predecrement r10 */
        blt unaligned_first_was_last

        evldd r5,8(r4)
        addic. r14,r14,-5
        evmergelohi r7,r6,r5
        evsubfw r3,r12,r7
        blt unaligned_loop_end

unaligned_double_loop:
        evandc r9,r11,r7
        evldd r6,16(r4)
        evand r9,r9,r3
        addi r4,r4,16
        evcmpgtu 6,r9,r0
        addi r10,r10,16
        evmergelohi r8,r5,r6
        bt 26,found_unaligned_null1
        evsubfw r3,r12,r8
        evstdd r7,0(r10)
        evandc r9,r11,r8
        evldd r5,8(r4)
        evand r9,r9,r3
        addic. r14,r14,-4
        evcmpgtu 7,r9,r0
        evmergelohi r7,r6,r5
        bt 30,found_unaligned_null2
        evsubfw r3,r12,r7
        evstdd r8,8(r10)

        evandc r9,r11,r7
        evldd r6,16(r4)
        evand r9,r9,r3
        addi r4,r4,16
        evcmpgtu 6,r9,r0
        addi r10,r10,16
        evmergelohi r8,r5,r6
        bt 26,found_unaligned_null1
        evsubfw r3,r12,r8
        evstdd r7,0(r10)
        evandc r9,r11,r8
        evldd r5,8(r4)
        evand r9,r9,r3
        evcmpgtu 7,r9,r0
        bt 30,found_unaligned_null2
        evmergelohi r7,r6,r5
        evstdd r8,8(r10)
        evsubfw r3,r12,r7
        bge unaligned_double_loop

/* Hit the end of the page, finish up the epilog of the loop, and
 * then run the process without any special scheduling to get
 * into the next page
 */
unaligned_loop_end:
        evandc r9,r11,r7
        addi r4,r4,16
        evand r9,r9,r3
        addi r10,r10,16
        evcmpgtu 6,r9,r0
        bt 26,found_unaligned_null1
        evstdd r7,0(r10)
        addi r10,r10,8

/* At this point, r4 and r10 point to the next place to load
 * and store.  We move forward, one word at a time, until r4
 * has passed over into a new page.
 */
unaligned_find_next_page:
        /* Check this word for null bytes */
        subf r3,r12,r5
        andc r9,r11,r5
        and. r9,r9,r3
        mr r0,r5
        bne find_null1
        stw r5,0(r10)
        addi r10,r10,4

        lwz r0,0(r4)
        addi r4,r4,4
        subf r3,r12,r0
        andc r9,r11,r0
        and. r9,r9,r3
        bne find_null1

        /* Calculate if r4 is on a new page */
        andi. r6,r4,0x0f00

        /* Store the 2nd word */
        stw r0,0(r10)
        addi r10,r10,4

        beq Unaligned_Double
        lwz r5,0(r4)
        addi r4,r4,4
        b unaligned_find_next_page

unaligned_first_was_last:
        addi r10,r10,16
        b unaligned_find_next_page

unaligned_copy:
        lbz r5, 0(r4)
        addi r10, r10, -1
        cmpwi cr1, r5,0
        beq cr1, Null_Byte1
unaligned_byte_loop:
        lbzu r6,1(r4)
        stbu r5,1(r10)
        cmpwi cr1, r5,0
        beq cr1,Null_Byte2
        lbzu r5,1(r4)
        stbu r6,1(r10)
        cmpwi cr1, r6,0
        bne cr1, unaligned_byte_loop

Null_Byte1:
        stb r5,1(r10)
        b Byte_End

Null_Byte2:
        stb r6,1(r10)

Byte_End:
        /* r3 still has original r10 value */
        lwz r14, 8(r1)
        addi r1,r1,16
        blr

        .size FUNCTION, .-FUNCTION

/* Data section */
        .sect ".data", "aw"
        .align 4
        .type fun_constants, @object
        .size fun_constants, 16
fun_constants:
        .long 0x01010101
        .long 0x01010101
        .long 0x80808080
        .long 0x80808080
