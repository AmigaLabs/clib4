/* memchr440.S, pl_string_common, pl_linux 10/11/04 11:45:34
 * ==========================================================================
 * Optimized memchr implementation for IBM PowerPC 405/440.
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
 * Function: Search for the first occurrence of character in buffer, checking
 *           at most 'count' bytes.
 *
 *           char *memchr(const char *buf, int c, size_t count)
 *
 * Input:  r3 - buffer address
 *         r4 - character in least significant byte of word
 *         r5 - count of bytes to search
 * Output: r3 - address of located character or NULL if not found
 *
 * ==========================================================================
 */

#include        "ppc4xx.inc"

#define DLMZBDOT(ra,rs,rb) .long 0x7c00009d|(rs<<21)|(ra<<16)|(rb<<11)

function_prolog(__memchr440)

    /*
     * Check count passed in R5. If zero, return null; otherwise continue.
	 */
	cmpwi	r5,0
    beq-    ret_null        /* Branch to return NULL if so             */


	addi 	r12,0,8		    /* Byte count of 8 into XER for 'load      */
	mtxer 	r12		        /* string' instruction later               */

	xor	r12,r12,r12	        /* Clear offset from the start             */

	rlwimi	r4,r4,8,16,23	/* Set up 'c' in all bytes of word.        */
    rlwimi.	r4,r4,16,0,15   /* Remember 'c' in LSB of int              */

    clrrwi  r8,r3,3         /* Find previous DW boundary               */
    addi    r8,r8,8         /* Find next DW boundary                   */
    subf    r8,r3,r8        /* Sub start addr from next DW boundary to */
                            /* get byte count to next DW boundary      */

    addic.  r9,r8,-8        /* If byte count = 8, we are DW aligned    */
    beq     again           /* So branch straight to fast loop         */
                            /* Otherwise, handle initial bytes one at  */
                            /* a time.				                   */
    andi.   r9,r4,255       /* Get search char only in low byte        */
    addi    r10,r3,-1       /* Preset base pointer for auto incr       */

next_byte:

    lbzu    r11,1(r10)      /* Load byte and update pointer to next    */
    addi    r12,r12,1       /* Bump the offset from the start          */
    xor.    r11,r11,r9      /* Branch if search char found             */
    beq-    found
    addic.  r5,r5,-1        /* Decrement the total count               */
	beq-	ret_null
    addic.  r8,r8,-1        /* Decrement the DW byte count             */
    bne+    next_byte       /* Loop again if still not DW aligned      */

again:  			        /* DW aligned, look for 'c' in new 8 bytes */

	lswx 	r6,r3,r12	    /* Load r6/r7 from r3 src + r12 offset     */
                            /* from the start                          */

	xor	r6,r6,r4	        /* XOR r4 & r6 to look for match           */
	xor	r7,r7,r4	        /* XOR r4 & r7 to look for match           */

	DLMZBDOT(0,6,7)         /* Check for hole created by match         */

    add	r12,r12,r0	        /* Update total offset with 1-8            */
    beq+    no_match        /* Branch if no match found in 8 bytes     */

	/* Match was found, but first must see if we've exceeded max count */									
									
    subf.   r5,r0,r5        /* Decrement total by DLMZB count 1-8      */
    blt     ret_null

found:

	addi    r12,r12,-1
	add	r3,r3,r12	        /* Return string start addr + offset to 'c'*/
	blr
	
no_match:

    subf.   r5,r0,r5        /* Decrement total by DLMZB count 1-8      */
    bgt+    again           /* If count > 0, read another 8 bytes      */

ret_null:

	xor	r3,r3,r3	        /* Return NULL since no match found        */
	blr

function_epilog(__memchr440)
