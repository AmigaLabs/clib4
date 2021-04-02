/* strncmp440.S, pl_string_common, pl_linux 10/11/04 11:45:43
 * ==========================================================================
 *
 * Optimized strncmp implementation for IBM PowerPC 405/440.
 *
 *  Copyright (c) 2003, IBM Corporation
 *  All rights reserved.               
 *                                     
 *  Redistribution and use in source and binary forms, with or      
 *  without modification, are permitted provided that the following 
 *  conditions are met:                                             
 *                                                                  
 *    * Redistributions of source code must retain the above        
 *      copyright notice, this list of conditions and the following 
 *      disclaimer.                                                 
 *    * Redistributions in binary form must reproduce the above     
 *      copyright notice, this list of conditions and the following 
 *      disclaimer in the documentation and/or other materials      
 *      provided with the distribution.                             
 *    * Neither the name of IBM nor the names of its contributors   
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.         
 *                                                                  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND          
 *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,     
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF        
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE        
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS   
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 *  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,            
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR  
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT        
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE   
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *                                                                          
 * ==========================================================================
 *
 * Function: Compare two character strings (up to n characters)
 *
 *           int strncmp(const char *s1, const char *s2, int n)
 *
 * Input:  r3 - string 1 address
 *         r4 - string 2 address
 *         r5 - maximum characters to compare
 * Output: r3 <0 (less), 0 (equal), >0 (greater)
 *
 * ==========================================================================
 */

#include        "ppc4xx.inc"	

#define DLMZBDOT(ra,rs,rb) .long 0x7c00009d|(rs<<21)|(ra<<16)|(rb<<11)

function_prolog(__strncmp440)

        /*
         * Check count passed in R5. If zero, return 0; otherwise continue.
	 */
	cmpwi	r5,0
	beq-    ret_0;

        /*
         * Most of the time the difference is found in the first
	 * several bytes.  The following code minimizes the number
	 * of load operations for short strings.
	 */
	mr	r11, r3		        /* Save string 2			*/

again:

	li	r9, -1		        /* 2nd word of dlmzb not used           */

	not	r10, r4		        /* string 1: bytes to page boundary	*/
	rlwinm.	r10, r10,29,23,31	/* string 1: dwords to page boundary	*/
	beq-	bytebybyte              /* If < 8 bytes to page boundary,       */
                                        /* handle bytewise                      */
	lwz	r8, 0(r4)		/* load 1st string 1 word		*/

	not	r12, r11		/* string 2: bytes to page boundary	*/
	rlwinm.	r12, r12,29,23,31	/* string 2: dwords to page boundary	*/
	beq-	bytebybyte              /* If < 8 bytes to page boundary,       */
                                        /* handle bytewise                      */
	lwz	r6, 0(r11)		/* load 1st string 2 word		*/

	DLMZBDOT(3,8,9)			/* quick zero check	                */

	bne+	partial

	subfic	r3, r5, 4		/* prepare counter		*/
	cmpwi	r3, 0			/* counter < 4 ?		*/
	bge+	first4                  /* Handle special if so         */

	cmplw   r8, r6  	        /* compare string 1 and string 2*/
	bne+    all_done		/* different => we're done	*/

	lwzu	r9, 4(r4)		/* load 2nd string 1 word      	*/
	lwzu	r7, 4(r11)		/* load 2nd string 2 word      	*/

	DLMZBDOT(3,8,9)			/* quick zero check		*/

	bne+	partial                 /* Handle special if string end */

	cmpwi	r5, 8			/* counter < 8 ?		*/
	blt	partial                 /* Handle special if < 8 bytes  */

	cmplw   r9, r7	                /* compare string 1 and string 2*/
	bne+    all_done		/* different => we're done	*/

	addi	r5, r5, -8		/* DWG */
	addi	r10, r4, 0x0004		/* DWG */
	not	r10, r10		/* DWG */
	rlwinm. r10, r10,29,23,31	/* DWG */
	addi	r12, r11, 0x0004	/* DWG */
	not	r12, r12		/* DWG */
	rlwinm. r12, r12,29,23,31	/* DWG */

        /* The following section prior to loop: figures out whether     */
        /* the string 1 or string  2 is closer to the page boundary.    */
        /* The main loop count is then set up to reflect the number of  */
        /* double words of the string  that is closest                  */

	cmpw	r10, r12		/* take minimum			*/
	blt	lt

	mr	r10, r12

lt:

	srwi	r12, r5, 3
	cmpw	r10, r12		/* ...and the counter		*/
        blt	lt2

	mr 	r10, r12		/* Changed from mr. to mr   DWG */
        /*beqlr-*/			/* Removed. DWG */

lt2:
	cmpwi	r10, 0			/* DWG */
	bne	lt3			/* DWG */
	addi	r4, r4, 0x0004		/* DWG */
	addi	r11,r11,0x0004		/* DWG */
	b	again			/* DWG */
lt3:					/* DWG */
	mtctr	r10			/* dword count for loop		*/
	lwzu	r6, 4(r11)		/* pre-load string 1   		*/
                                        /* To the loop                  */
	b	in

loop:					/* main loop			*/

	cmplw   r8, r6                  /* Compare 1st string 2 word    */
	bne-    all_done	        /* with first string 1 word     */
                                        /* If different, we're done     */
	cmplw   r9, r7                  /* Compare 2nd string 2 word    */
                                        /* with second string 1 word    */
	lwzu	r6, 4(r11)              /* Preload string 1 word        */

	bne-    all_done                /* If different, we're done     */

in:

	lwzu	r7, 4(r11)              /* Load string 1 word           */
	lwzu	r8, 4(r4)               /* Load string 2 word           */

	DLMZBDOT(3,6,7)                 /* Zero check                   */

	lwzu	r9, 4(r4)               /* Load string 2 word           */
	bdnzt+	eq, loop

	mfctr	r12			/* number of dwords left	*/
	subf	r10, r12, r10	        /* number of dwords compared	*/
	slwi	r10, r10, 3
	subf	r5, r10, r5	        /* adjust byte counter		*/
	beq-	lt4			/* DWG */
	addi	r5, r5, 8		/* DWG */
	b	partial                 /* DWG Handle special if < 8    */

lt4:
	/*cmpwi	r5, 8*/ /*DWG*/
	/*blt	partial*/ /*DWG*/       /* Handle special if < 8        */

	/*addic	r5, r5, -8*/		/* Removed.                 DWG */

	cmplw   r8, r6  	        /* compare last dword		*/
	addi	r4, r4, 4
	bne-    all_done

	cmplw   r9, r7
	addi	r11, r11, 4
	bne-    all_done

bytebybyte:

	/* We've got close to a page boundary: do a byte-byte-byte
	 * compare for the following 8 bytes, and then go back to
	 * the full-word compare loop.
	 */

	li	r3, 8			/* loop count			*/
	cmpw	r3, r5		        /* take min(8, counter)		*/
	ble	f2

	mr.	r3, r5

	beqlr

f2:

	mtctr	r3
	subf	r5, r3, r5	        /* adjust counter		*/

bbb:

	lbz	r6, 0(r11)		/* byte copy loop		*/

	addi	r11, r11, 1

	lbz	r8, 0(r4)

	addi	r4, r4, 1
	cmpwi	cr1, r6, 0

	cmplw   r8, r6
	beq	cr1,final

	bdnzt+	eq, bbb
	bne     all_done

	cmpwi	r5, 0
	bgt	again   		/* handle the rest		*/

        xor     r3,r3,r3

	blr

final:

        bne     all_done
ret_0:
        xor     r3,r3,r3                /* Equal case                   */
        blr

partial:
	/*
         * Zero byte found.  Align the loaded data to the
	 * zero byte position and compare the results.
	 */

	cmpw	r3, r5		        /* take min(pos, counter)	*/
	blt	f1

	mr.	r3, r5

	beqlr

f1:

	subfic	r3, r3, 4		/* zero/end in first word?	*/
	cmpwi	r3, 0
	blt	last4

first4:

	rlwinm	r3, r3, 3, 0, 31	/* count *= 8			*/
	srw	r6, r6, r3	        /* align 1st string 2 word     	*/
	srw	r8, r8, r3	        /* align 1st string 1 word     	*/

	cmplw   r8, r6  	        /* get result			*/
        bne     all_done
        xor     r3,r3,r3                /* Equal case                   */
	blr

last4:

	addi	r10, r3, 4		/* adjust count			*/

	cmplw   r8, r6  	        /* 1st word compare		*/
	bne     all_done

	rlwinm	r10, r10, 3, 0, 31	/* count *= 8			*/
	srw	r7, r7, r10	        /* align 2nd string 2 word     	*/
	srw	r9, r9, r10	        /* align 2nd string 1 word     	*/

	cmplw   r9, r7  	        /* get result			*/
        bne     all_done
        xor     r3,r3,r3                /* Equal case                   */
	blr

all_done:

        blt     finish_lt

        addi    r3,0,-1                 /* Less than case               */

        blr

finish_lt:

        addi    r3,0,1                  /* Greater than case            */

        blr

function_epilog(__strncmp440)
