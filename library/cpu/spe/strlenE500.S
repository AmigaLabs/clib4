/*------------------------------------------------------------------
 * strlen.S
 *
 * Standard strlen function optimized for e500 using SPE
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
 * int strlen(const char *src)
 *
 * Returns:
 * length of string up to first null byte
 *------------------------------------------------------------------
 */

	.file   "strlen.S"
	.section        ".text"
	.align 4
#define FUNCTION __strlen_e500
	.globl FUNCTION
	.type FUNCTION,@function
FUNCTION:
        /* check enough bytes to align the src pointer to a
	 * doubleword boundary, and then run through the bytes
	 * until a null is detected, adding up the length as we
	 * go.
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

        /* Put the address of the constants in
	 * r11 (we'll overwrite it when we load 0x8080...)
	 * This uses a relocation to patch the source register to
	 * be one of r0, r2, or r13 depending on the small data
	 * section into which fun_constants gets located
	 */
        lis r11,fun_constants@ha
        addi r11,r11, fun_constants@l

        /* r3 will be used for returning length, so lets move src
	 * out of there to r10
	 */
        mr r10,r3

        /* Get src's alignment (within a doubleword), and check
	 * if it's zero
	 */
        andi. r3,r3,0x7

        /* Load the 0x01010101 constant */
        evldd r12,0(r11)

        /* Clear r0 out */
        evsplati r0,0

        /* Load the 0x80808080 constant */
        evldd r11,8(r11)

        /* get (the word alignment of r10) * 8 to shift a mask by the
	 * number of bytes which are unused by r10
	 * This is: clrlslwi r6,r10,30,3
	 */
        rlwinm r6, r10, 3, 27,28

        /* Skip out now if r10 was aligned. */
        beq Aligned_Double

        subfic r6, r6, 32

        /* start the mask with all Fs */
        li r5, -1

        /* Shift r8 by r6 to mask off preceding bytes */
        slw r8, r5, r6

        /* We want the result of r3+r10 to be a
	 * doubleword-aligned value, so r3 will be -1*(r10)
	 * alignment)
	 */
        neg r3,r3

/* load the first words (on word boundaries) and process them,
 * setting bits preceding the first byte to 1.
 */

        /* Grab the first doubleword of r10 */
        evlddx r4,r3,r10

        /* The loop is usually ahead by 16, and if we detect a
	 * null, the exit code assumes that r3 is ahead by
	 * 16, so we add 16 now, and can subtract 8 if there was
	 * no null
	 */
        addi r3,r3,16

        mtcrf 0x01,r10

        evmergelo r0,r8,r0
        cror 28,29,29

        evmergelo r5,r5,r8

        evsel r8,r5,r0,cr7

        evsplati r0,0

        /* Set the unwanted bytes of the source */
        evor r4, r4, r8

        /* Process fist doubleword with fun algorithm */
        evsubfw r7, r12, r4
        evandc r9, r11, r4
        evand r9, r7, r9
        evcmpgtu 1,r9,r0

        /* Skip out if there was a null */
        bt 6, found_null1

        /* There was no null; correct the length */
        addi r3,r3,-8

        /* loop through 2 doublewords at once in this page, until one of
	 * these happens:
	 * 1) A null byte is found in src
	 * 2) the end of the page is reached
	 *
	 * If 2 happens, then we finish processing the last doubleword in
	 * the page, and if it checks out, we jump to the next one.  The
	 * hope is that no small strings are going to cross page
	 * boundaries.  For large strings, the hit should be minimal
	 */

        /* The main loop is unrolled, and software pipelined to
	 * get as close to 2 IPC as possible.  This means it has
	 * a prolog and epilog that need to be executed.  Prolog
	 * is:
	 * load, subtract
	 */
Aligned_Double:
        /* Start figuring out how far to the next page */
        not r8,r10
        evlddx r4,r3,r10

        /* Find the distance (in doublewords) from r10 to the end
	 * of the page
	 */
        rlwinm r8,r8,29,23,31

        /* Find out if this is the last double on the page */
        addic. r6, r8,-1
        addi r3,r3,8 /* increment the length */

        /* start calculating now, since the loop needs this at
	 * the beginning.
	 */
        evsubfw r7, r12, r4
        beq last_double_was_first

        /* decrement the counter to account for the first
	 * iteration of the loop, and skip to the end if it turns
	 * out we don't have enough doubles left for 1 iter.
	 */
        addic. r6,r6,-2
        blt aligned_double_loop_end
        /* The loop */
aligned_double_loop:
        evandc r9, r11, r4
        evlddx r5,r3,r10
        evand r9,r9,r7
        addi r3,r3,8
        evcmpgtu 1, r9, r0
        addic. r6,r6,-2
        evsubfw r7,r12,r5
        bt 6, found_null1
        evandc r9, r11, r5
        evlddx r4,r3,r10
        evand r9, r9, r7
        addi r3,r3,8
        evcmpgtu 5, r9, r0
        bt 22, found_null2
        evsubfw r7,r12, r4
        bge aligned_double_loop

        /* we exited the loop, we must be at the end of a page.
	 * Finish the last one, and then see if we are done or
	 * not.
	 */
aligned_double_loop_end:
        evandc r9,r11,r4
        evand r9,r9,r7
        evcmpgtu 1,r9,r0

        /* r3 has to be ahead by 16 in found_null, and is
	 * only ahead by 8 right now, so we add 8 here
	 */
        addi r3,r3,8
        bt 6, found_null1

        /* However, r3 needed to be only 8 ahead to load the
	 * next doubleword, so take 8 away again
	 */
        addi r3,r3,-8

        /* To make things nice, we finish off the remaining
	 * doublewords here, so that the next loop has exactly
	 * 512 iterations
	 */
        addi r7, r10, r3
        andi. r7,r7,0xFFF
        beq start_new_loop
find_page_end:
        evlddx r4,r3,r10
        addi r3,r3,16 /* overadjust */
        evsubfw r7, r12, r4
        evandc r9, r11, r4
        evand r9, r7, r9
        evcmpgtu 1, r9, r0
        bt 6, found_null1
        addi r3,r3,-8 /* readjust */
        addi r7,r10,r3
        andi. r7,r7,0xFFF
        bne find_page_end

start_new_loop:
        /* It is now safe to proceed to the next page.  load the
	 * counter with the number of doublewords in a page
	 * (minus 4 to account for the first iteration)
	 */
        evlddx r4,r3,r10
        li r6, 508

        evsubfw r7, r12, r4
        addi r3,r3,8
        b aligned_double_loop

last_double_was_first:
        evandc r9,r11,r4
        evand r9,r9,r7
        evcmpgtu 1,r9,r0
        addi r3,r3,8 /* adjust */
        bt 6,found_null1
        addi r3,r3,-8 /*readjust */
        b start_new_loop


        /* The first doubleword had a null byte */
found_null1:
        /* Compensate for the fact that r3 is 16 ahead */
        addi r3,r3,-12

        /* If there was a null in the hi word, move that word
	 * down into the lo word, and subtract 4 from r3
	 */
        bf 4, find_null1
        evmergehi r4, r4, r4
        addi r3,r3,-4
find_null1:
        rlwinm. r11,r4,0,0,7
        beq finish1
        addi r3,r3,1
        rlwinm. r11,r4,0,8,15
        beq finish1
        addi r3,r3,1
        rlwinm. r11,r4,0,16,23
        beq finish1
        addi r3,r3,1

finish1:
        blr

        /* The second doubleword had a null byte */
found_null2:
        /* Compensate for the fact that r3 is 16 ahead */
        addi r3,r3,-12

        /* If there was a null in the hi word, move that word
	 * down into the lo word, and subtract 4 from r3
	 */
        bf 20, find_null2
        evmergehi r5, r5, r5
        addi r3,r3,-4
find_null2:
        rlwinm. r11,r5,0,0,7
        beq finish2
        addi r3,r3,1
        rlwinm. r11,r5,0,8,15
        beq finish2
        addi r3,r3,1
        rlwinm. r11,r5,0,16,23
        beq finish2
        addi r3,r3,1

finish2:
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
