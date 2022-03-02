/* __strlen440.S, pl_string_common, pl_linux 10/11/04 11:45:42
 * ==========================================================================
 *
 * Optimized strlen implementation for IBM PowerPC 405/440.
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
 * Function: Determines the length of a string
 *
 *           size_t strlen(const char *s1)
 *
 * Input:  r3 - string address
 * Output: r3 - Length of string, excluding NULL terminator
 *
 * ==========================================================================
 */

#include        "ppc4xx.inc"

#define DLMZBDOT(ra,rs,rb) .long 0x7c00009d|(rs<<21)|(ra<<16)|(rb<<11)

function_prolog(__strlen440)
	xor	r12,r12,r12	        /* Clear total offset from the start      */
	neg	r9,r3     	        /* get 2's complement of input address    */
	rlwinm.	r5,r9,0,29,31   /* calc number of bytes to dword boundary */
	beq-	dwbndry         /* if zero, then already on dword bndry   */

	mtctr	r5              /* do byte by byte until dword bndry      */
bytebybyte:
	lbzx    r4,r12,r3       /* load byte                              */
	addi	r12,r12,1       /* bump offset                            */
	cmpwi	r4,0            /* compare to NULL                        */
	bdnzf	eq,bytebybyte   /* loop till zero count or NULL found     */
	beq-	found           /* check if NULL found                    */

dwbndry:
	rlwinm.	r5,r9,29,30,31  /* calc # of dwords to quad word bndry    */
	beq	qwbndry             /* if zero, then already on qword bndry   */
	addi 	r5,0,8		    /* Byte count of 8 into XER for 'load     */
        				    /* string' instruction later              */
    mtxer   r5              /* Load count for 'load string'           */
	lswx 	r6,r3,r12	    /* Load r6-r7 from r3 src + r12 offset    */
	DLMZBDOT(11,6,7)        /* Look for NULL in these 8 bytes         */
    add     r12,r12,r11     /* Bump the offset from the start         */
	bne-	found		    /* If NULL found, done; else keep looking */

qwbndry:
	addi 	r5,0,16		    /* Byte count of 16 into XER for 'load    */
				            /* string' instruction later		      */

again:		                /* New 16 bytes, look for NULL             */
    mtxer   r5              /* Load count for 'load string'            */
	lswx 	r6,r3,r12	    /* Load r6-r9 from r3 src + r12 offset     */
                            /* from the start                          */
	DLMZBDOT(11,6,7)        /* Look for NULL in these first   8 bytes  */
    add     r12,r12,r11     /* Bump the offset from the start          */
	bne-	found		    /* If no NULL in first 8 bytes, fall       */
				            /* through and test next 8 bytes, else,    */
				            /* we're done				               */
    DLMZBDOT(11,8,9)        /* Look for NULL in these next    8 bytes  */
    add     r12,r12,r11     /* Bump the offset from the start          */
	beq+	again   	    /* If no NULL, grab next 16 bytes, else,   */
				            /* we're done.				               */

found:
	addi	r3,r12,-1       /* Return count less the NULL              */
	blr

function_epilog(__strlen440)
