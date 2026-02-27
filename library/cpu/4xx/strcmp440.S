/* __strcmp440.S, pl_string_common, pl_linux 10/11/04 11:45:40
 * ==========================================================================
 * Optimized strcmp implementation for IBM PowerPC 405/440.
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
 * Function: Compare two character strings
 *
 *           int strcmp(const char *s1, const char *s2) 
 *  
 * Input:  r3 - string 1 address
 *         r4 - string 2 address
 * Output: r3 <0 (less), 0 (equal), >0 (greater)
 *
 * ==========================================================================
 */

#include        "ppc4xx.inc"	

#define DLMZBDOT(ra,rs,rb) .long 0x7c00009d|(rs<<21)|(ra<<16)|(rb<<11)

function_prolog(__strcmp440)

        /* Most of the time the difference is found in the first
	 * several bytes.  The following code minimizes the number
	 * of load operations for short strings.  
	 */
         
	mr	r10, r3		            /* Save string 2       		*/
        
again:
	li	r8, -1		            /* 2nd word of dlmzb not used	*/
        
	not	r9, r4		            /* string 1: bytes to page bdy	*/
	rlwinm.	r9, r9,29,23,31	    /* string 1: dwords to page bdy	*/
	beq-	bytebybyte          /* If <8 bytes to page bdy      */
                                /* do byte by byte              */
	lwz	r7, 0(r4)		        /* load 1st string 1 word      	*/
        
	not	r11, r10		        /* string 2: bytes to page bdy	*/
	rlwinm.	r11, r11,29,23,31	/* string 2: dwords to page bdy	*/
	beq-	bytebybyte          /* If <8 bytes to page bdy      */
                                /* do byte by byte              */

	lwz	r5, 0(r10)		        /* load 1st string 2 word      	*/
        
	DLMZBDOT(3,7,8)			    /* quick zero check		*/
        
	subfic	r3, r3, 4		    /* adjust counter		*/
	bne+	first4              /* Branch if <4 bytes           */
        
        cmplw   r7, r5          /* compare string 1 and string 2*/
	bne+    all_done            /* different => we're done	*/

	lwzu	r7, 4(r4)		    /* load 2nd string 1 word      	*/
	lwzu	r5, 4(r10)		    /* load 2nd string 2 word      	*/
        
	DLMZBDOT(3,7,8)			    /* quick zero check		*/
        
	subfic	r3, r3, 4		    /* adjust counter		*/
	bne+	first4              /* Branch if <4 bytes           */
        
	cmplw   r7, r5	            /* compare string 1 and string 2*/
	bne+    all_done		    /* different => we're done	*/

	addi	r9, r4, 0x0004		/* DWG*/
	not	r9, r9			        /* buffer 2: bytes to page bdy DWG */
	rlwinm.	r9, r9,29,23,31		/* buffer 2: dwords to page bdy	DWG */
	addi	r11, r10, 0x0004	/* DWG */
	not	r11, r11		        /* buffer 1: bytes to page bdy DWG */
	rlwinm.	r11, r11,29,23,31	/* buffer 1: dwords to page bdy	DWG */

        /* The following section prior to loop: figures out whether     */
        /* the string 1 or string  2 is closer to the page boundary.    */
        /* The main loop count is then set up to reflect the number of  */
        /* double words of the string  that is closest                  */

        cmpw	r9, r11		    /* take minimum			*/
	blt	lt
        
	mr	r9, r11
lt:
	cmpwi	r9, 0			    /* DWG */
	bne	lt2			            /* DWG */
	addi	r4, r4, 0x0004		/* DWG */
	addi	r10,r10,0x0004		/* DWG */
	b	again			        /* DWG */
lt2:					        /* DWG */
	mtctr	r9			        /* dword count for loop 	*/
	lwzu	r5, 4(r10)		    /* pre-load string 1 word	*/
                                /* To the loop                  */
	b	in
        
loop:					        /* main loop			*/

	cmplw   r7, r5              /* Compare 1st string 2 word    */
    bne-    all_done	        /* with first string 1 word     */
                                /* If different, we're done     */
	cmplw   r8, r6              /* Compare 2nd string 2 word    */
                                /* with 2nd string 1 word       */
    lwzu	r5, 4(r10)          /* Preload string 1 word        */
        
	bne-    all_done            /* If different, we're done     */
        
in:

	lwzu	r6, 4(r10)          /* Load string 1 word           */
	lwzu	r7, 4(r4)           /* Load string 2 word           */
        
	DLMZBDOT(3,5,6)             /* zero check                   */
        
	lwzu	r8, 4(r4)           /* Load string 2 word           */
	bdnzt+	eq, loop            /* dec cnt, continue loop if    */
                                /* count not 0 and no strnig end*/
	bne+	partial

	cmplw   r7, r5	            /* compare last dword      	*/
	addi	r4, r4, 4
	bne-    all_done
        
	cmplw   r8, r6
	addi	r10, r10, 4
	bne-    all_done

bytebybyte:

	/* 
     * We've got close to a page boundary: do a byte-byte-byte
	 * compare for the following 8 bytes, and then go back to
	 * the full-double word compare loop.
	 */
         
	lbz	r5, 0(r10)
	lbz	r7, 0(r4)
	cmpwi	cr1, r5, 0
	beq-	cr1, s2
        
	cmplw   r7, r5              /* compare bytes		*/
	lbzu    r5, 1(r10)		    /* load next string 2 byte     	*/
	bne-    all_done		    /* bytes differ, we're done	*/
	lbzu    r7, 1(r4)		    /* load next string 1 byte     	*/
	cmpwi   cr1, r5, 0		    /* zero?			*/
	beq-	cr1, s2
        
	cmplw   r7, r5	            /* compare bytes		*/
	lbzu    r5, 1(r10)		    /* load next string 2 byte     	*/
	bne-    all_done		    /* bytes differ, we're done	*/
	lbzu    r7, 1(r4)		    /* load next string 1 byte     	*/
	cmpwi	cr1, r5, 0		    /* zero?			*/
	beq-	cr1, s2
        
	cmplw   r7, r5	            /* compare bytes		*/
	lbzu    r5, 1(r10)	       	/* load next string 2 byte     	*/
	bne-    all_done        	/* bytes differ, we're done	*/
	lbzu    r7, 1(r4)		    /* load next string 1 byte     	*/
	cmpwi   cr1, r5, 0		    /* zero?			*/
	beq-	cr1, s2
        
	cmplw   r7, r5	            /* compare bytes		*/
	lbzu    r5, 1(r10)		    /* load next string 2 byte     	*/
	bne-    all_done		    /* bytes differ, we're done	*/
	lbzu    r7, 1(r4)		    /* load next string 1 byte     	*/
	cmpwi	cr1, r5, 0		    /* zero?			*/
	beq-	cr1, s2
        
	cmplw   r7, r5	            /* compare bytes		*/
	lbzu    r5, 1(r10)		    /* load next string 2 byte     	*/
	bne-    all_done        	/* bytes differ, we're done	*/
	lbzu    r7, 1(r4)		    /* load next string 1 byte     	*/
	cmpwi	cr1, r5, 0		    /* zero?		        */
	beq-	cr1, s2
        
	cmplw   r7, r5	            /* compare bytes		*/
	lbzu    r5, 1(r10)		    /* load next string 2 byte    	*/
	bne-    all_done		    /* bytes differ, we're done	*/
	lbzu    r7, 1(r4)		    /* load next string 1 byte     	*/
	cmpwi	cr1, r5, 0		    /* zero?			*/
	beq-	cr1, s2
        
	cmplw   r7, r5	            /* compare bytes		*/
	lbzu    r5, 1(r10)		    /* load next string 2 byte     	*/
	bne-    all_done		    /* bytes differ, we're done	*/
	lbzu    r7, 1(r4)		    /* load next string 1 byte    	*/
	cmpwi	cr1, r5, 0		    /* zero?			*/
	beq-	cr1, s2
        
	cmplw   r7, r5	            /* compare bytes		*/

	beqlr	cr1			        /* zero found, we're done	*/
        
	addi	r4, r4, 1
	bne     all_done		    /* bytes differ, we're done	*/
        
	addi	r10, r10, 1
        
	b	again                   /* Back to DW loop              */
        
s2:

	cmplw   r7, r5	            /* get result			*/
	bne     all_done
        
        xor     r3,r3,r3        /* Strings are same             */
        blr

partial:

	/* 
     * Zero byte found.  Align the loaded data to the
	 * zero byte position and compare the results.
	 */
         
	subfic	r3, r3, 4		    /* zero in first word?		*/
	blt	last4
        
first4:

	rlwinm	r3, r3, 3, 0, 31	/* count *= 8			*/
        
	srw	r5, r5, r3	            /* align 1st string 2 word     	*/
	srw	r7, r7, r3	            /* align 1st string 1 word     	*/
        
	cmplw   r7, r5	            /* get result			*/
        bne     all_done
        
        xor     r3,r3,r3        /* Strings are same             */
        blr
        
last4:

	addi	r9, r3, 4		    /* adjust count			*/
        
	cmplw   r7, r5	            /* 1st word compare		*/
	bne     all_done		    /* words differ, we're done	*/
        
	rlwinm	r9, r9, 3, 0, 31	/* count *= 8			*/
        
	srw	r6, r6, r9	            /* align 2nd string 2 word     	*/
	srw	r8, r8, r9	            /* align 2nd string 1 word     	*/
        
	cmplw   r8, r6	            /* get result			*/
        bne     all_done

        xor     r3,r3,r3        /* Strings are same             */
        blr
        
all_done:

        blt     finish_lt       /* Less than result             */
        
        addi    r3,0,-1
        
        blr
        
finish_lt:

        addi    r3,0,1          /* Greater than result          */
        
        blr

function_epilog(__strcmp440)
