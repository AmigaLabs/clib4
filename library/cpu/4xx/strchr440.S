/* strchr440.S, pl_string_common, pl_linux 10/11/04 11:45:39
 * ==========================================================================
 *
 * Optimized strchr implementation for IBM PowerPC 405/440.
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
 * Function: Search for first occurrence of character in string. The character 
 *           can be the NULL character and the NULL character in the search 
 *           string is included in the search. 
 *
 *           char *strchr(const char *s1, int c) 
 * 
 * Input:  r3 - string address
 *         r4 - character in least significant byte of word 
 * Output: r3 - address of located character or NULL if not found
 *
 * ==========================================================================
 */

#include        "ppc4xx.inc"

#define DLMZBDOT(ra,rs,rb) .long 0x7c00009d|(rs<<21)|(ra<<16)|(rb<<11)
           
function_prolog(__strchr440)
	cmpwi	r4,0
	beq-	char_is_null	/* Search char is NULL, so do strlen op    */

	xor	r5,r5,r5	        /* Clear total offset from start           */
	neg	r9,r3     	        /* get 2's complement of input address     */
	rlwinm.	r7,r9,0,29,31   /* calc number of bytes to dword boundary  */
	beq-	dwbndry         /* if zero, then already on dword bndry    */

	mtctr	r7              /* do byte by byte until dword bndry       */
bytebybyte:
	lbzx    r6,r5,r3        /* load byte                               */
	addi	r5,r5,1         /* bump offset                             */
	cmpwi	r6,0            /* compare to NULL                         */
	beq-	ret_null        /* found end of string with no match       */
	cmpw	r6,r4
	beq-	found
	bdnz	bytebybyte      /* loop till zero count                    */

dwbndry:
	addi 	r6,0,8		/* Byte count of 8 in XER for 'load        */
	mtxer 	r6		    /* string' instruction later               */
        
	rlwimi	r4,r4,8,16,23	/* Set up 'c' in all bytes of word.        */
    rlwimi.	r4,r4,16,0,15   /* Remember 'c' in LSB of int              */

	b	again

comp:			        /* No NULL in eight bytes, look for 'c'    */

	xor	r6,r6,r4	    /* XOR r4 & r6 to look for match           */
	xor	r7,r7,r4	    /* XOR r4 & r7 to look for match           */
        
	DLMZBDOT(0,6,7)     /* Check for hole created by match         */
        
	add	r5,r5,r0	    /* Update total offset with 1-8            */
        
	bne-	found	    /* If no hole, fall through and get next 8 */
                        /* bytes, otherwise, go to logic to handle */
                        /* 8 bytes w/ match                        */

again:			        /* New 8 bytes, look for NULL first        */

	lswx 	r6,r3,r5	/* Load r6/r7 from r3 src + r5 offset      */
                        /* from the start                          */
	DLMZBDOT(12,6,7)	/* Look for NULL in these current 8 bytes  */
	beq+	comp		/* If no NULL, search 8 bytes for 'c'      */

   /* 
    * Here we found a NULL because of end of string, but we don't know 
    * if there is a match for 'c' in the bytes prior to the NULL. 
    */

	xor     r6,r6,r4	/* XOR r4 & r6 to look for match           */
    xor     r7,r7,r4	/* XOR r4 & r7 to look for match           */
        
    DLMZBDOT(0,6,7)		/* Check for hole created by match         */
        
    cmplw   r0,r12		/* Was there a match before the NULL?      */
    bge     ret_null	/* If not, return NULL                     */
    add     r5,r5,r0	/* Else, update the offset from the start  */
        
   /* 
    * Here we know the string is not ending, but we found a match in 
    * these 8 bytes or... we found a string ending NULL, but then found  
    * a match before this NULL.
    */

found:

	addi    r5,r5,-1
	add	r3,r3,r5	        /* Return string start addr + offset to 'c'*/
	blr
        
ret_null:

	xor	r3,r3,r3	        /* Return NULL since no match found        */
	blr

char_is_null:			    /* Since search char is NULL, we basically */
				            /* do a strlen operation                   */

	xor	r12,r12,r12	        /* Clear total offset from the start       */
	neg	r9,r3     	        /* get 2's complement of input address     */
	rlwinm.	r7,r9,0,29,31   /* calc number of bytes to dword boundary  */
	beq-	s_dwbndry       /* if zero, then already on dword bndry    */

	mtctr	r7              /* do byte by byte until dword bndry       */
s_bytebybyte:
	lbzx    r6,r12,r3       /* load byte                               */
	addi	r12,r12,1       /* bump offset                             */
	cmpwi	r6,0            /* compare to NULL                         */
	bdnzf	eq,s_bytebybyte /* loop till zero count or NULL found      */
	beq-	s_found         /* check if NULL found                     */

s_dwbndry:
	rlwinm.	r7,r9,29,30,31  /* calc # of dwords to quad word bndry     */
	beq	s_qwbndry           /* if zero, then already on qword bndry    */
	addi 	r7,0,8		    /* Byte count of 8 into XER for 'load      */
				            /* string' instruction later		   */
    mtxer   r7              /* Load count for 'load string'            */
	lswx 	r6,r3,r12	    /* Load r6-r7 from r3 src + r12 offset     */
	DLMZBDOT(11,6,7)        /* Look for NULL in these 8 bytes          */
    add     r12,r12,r11     /* Bump the offset from the start          */
	bne-	s_found		    /* If NULL found, done; else keep looking  */

s_qwbndry:
	
    addi 	r5,0,16		    /* Byte count of 16 into XER for 'load     */
				            /* string' instruction later		   */
     
s_again:	                /* New 16 bytes, look for NULL             */

    mtxer   r5              /* Load count for 'load string'            */
	lswx 	r6,r3,r12	    /* Load r6-r9 from r3 src + r12 offset     */
                            /* from the start                          */
	DLMZBDOT(11,6,7)        /* Look for NULL in these first   8 bytes  */
        
    add     r12,r12,r11     /* Bump the offset from the start          */

	bne-	s_found		    /* If no NULL in first 8 bytes, fall       */
				            /* through and test next 8 bytes, else,    */
				            /* we're done				               */

    DLMZBDOT(11,8,9)        /* Look for NULL in these next    8 bytes  */

    add     r12,r12,r11     /* Bump the offset from the start          */

	beq+	s_again   	    /* If no NULL, grab next 16 bytes, else,   */
				            /* we're done.				               */
s_found:

	addi	r12,r12,-1      /* Compensate for NULL                     */
 	add	r3,r12,r3           /* Add returned length to string pointer   */

	blr
	
function_epilog(__strchr440)
