/* __strcpy440.S, pl_string_common, pl_linux 10/11/04 11:45:41 
 * ==========================================================================
 *
 * Optimized strcpy implementation for IBM PowerPC 405/440.
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
 * Function: Copies string s2 to location s1. Behavior is undefined for 
 *           objects that overlap. 
 *
 *           char *strcpy(const char *s1, const char *s2) 
 *  
 * Input:  r3 - destination string address
 *         r4 - source string address 
 * Output: r3 - address of destination string
 *
 * ==========================================================================
 */

#include        "ppc4xx.inc"	

#define DLMZBDOT(ra,rs,rb) .long 0x7c00009d|(rs<<21)|(ra<<16)|(rb<<11)
                  
function_prolog(__strcpy440)

	xor	r12,r12,r12	/* Clear total offset from the start */

	neg	r9,r4     	/* get 2's complement of src address. DWG */
	rlwinm.	r5,r9,0,29,31   /* calc number of bytes to dword boundary  */
	beq-	dwbndry         /* if zero, then already on dword bndry */

	mtctr	r5              /* do byte by byte until dword bndry */
bytebybyte:
	lbzx    r6,r12,r4       /* load byte */
	stbx	r6,r12,r3
	addi	r12,r12,1       /* bump offset */
	cmpwi	r6,0            /* compare to NULL */
	bdnzf	eq,bytebybyte   /* loop till zero count or NULL found */
	beq-	found           /* check if NULL found */

dwbndry:
	rlwinm.	r5,r9,29,30,31  /* calc # of dwords to quad word bndry   */
	beq	qwbndry         /* if zero, then already on qword bndry */
	addi 	r5,0,8		/* Byte count of 8 into XER for 'load     */
				/* string' instruction later		   */		
        mtxer   r5              /* Load count for 'load string'            */
	lswx 	r6,r4,r12	/* Load r6-r7 from r4 src + r12 offset     */
	DLMZBDOT(0,6,7)         /* Look for NULL in these 8 bytes  */
	bne-	null_found	/* If NULL found, done; else keep looking */
        stswx   r6,r3,r12       /* Store                                   */
        add     r12,r12,r0      /* Bump the offset from the start          */

qwbndry:
	addi 	r5,0,16		/* Byte count of 16 in XER for 'load       */
              			/* string' instruction                     */
        b       again		/* To main loop 			   */
        
store:
        
        DLMZBDOT(10,8,9)        /* Look for NULL in second 8 bytes         */
        add     r0,r0,r10       /* Add to offset from this DLMZB           */
        bne-    null_found      /* Branch if end of string found           */        
        mtxer   r5              /* Count of 16 in XER for 'load string'    */
        stswx   r6,r3,r12       /* Store four words                        */
        add     r12,r12,r0      /* Bump offset from the start and fall     */
        			/* into logic to read another 16 bytes     */
again:
        mtxer   r5              /* Count of 16 in XER for load string      */  
        xor     r0,r0,r0        /* Running offset from the start           */
    	lswx 	r6,r4,r12	/* Load r6-r9 from r4 src + r12 offset     */
                                /* from the start                          */
       	DLMZBDOT(10,6,7)        /* Look for NULL in these first 8 bytes    */
        add     r0,r0,r10       /* Add to offset from this DLMZB           */
        beq+    store           /* Loop back if no end of string found     */
        mtxer   r0              /* Count from DLMZBs                       */
        stswx   r6,r3,r12       /* Store partial from first two words      */
        blr                     /* Return dest ptr which was unchanged     */
        
null_found:
        mtxer   r0              /* Count from DLMZBs                       */
        stswx   r6,r3,r12       /* Store from first two words and          */
                                /* partial from second two words           */
found:
	blr                     /* Return dest ptr which was unchanged     */

function_epilog(__strcpy440)
