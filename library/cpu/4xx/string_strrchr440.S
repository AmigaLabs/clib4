/* strrchr440.S, pl_string_common, pl_linux 10/11/04 11:45:44 
 * ==========================================================================
 *
 * Optimized strrchr implementation for IBM PowerPC 405.
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
 * Function: Search for last occurrence of character in string. The character 
 *           can be the NULL character and the NULL character in the search 
 *           string is included in the search. 
 *
 *           char *strrchr(const char *s1, int c) 
 *  
 * Input:  r3 - string address
 *         r4 - character in least significant byte of word 
 * Output: r3 - address of located character or NULL if not found
 *
 * ==========================================================================
 */

#include        "ppc4xx.inc"	

#define DLMZBDOT(ra,rs,rb) .long 0x7c00009d|(rs<<21)|(ra<<16)|(rb<<11)
           
function_prolog(__strrchr440)

	/* Since we are interested in the "last" occurrence of the search  */
	/* char, this first section of code deos a strlen operation to     */
	/* locate the end of string. Then we work backwards from there.    */      
	
	xor	r12,r12,r12	/* Clear total offset from the start       */

	neg	r9,r3     	/* get 2's complement of input address     */
	rlwinm.	r5,r9,0,29,31   /* calc number of bytes to dword boundary  */
	beq-	dwbndry         /* if zero, then already on dword bndry    */

	mtctr	r5              /* do byte by byte until dword bndry       */
bytebybyte:
	lbzx    r6,r12,r3       /* load byte                               */
	addi	r12,r12,1       /* bump offset                             */
	cmpwi	r6,0            /* compare to NULL                         */
	bdnzf	eq,bytebybyte   /* loop till zero count or NULL found      */
	beq-	found           /* check if NULL found                     */

dwbndry:
	rlwinm.	r5,r9,29,30,31  /* calc # of dwords to quad word bndry     */
	beq	qwbndry         /* if zero, then already on qword bndry    */
	addi 	r5,0,8		/* Byte count of 8 into XER for 'load      */
				/* string' instruction later		   */
	mtxer   r5              /* Load count for 'load string'            */
	lswx 	r6,r3,r12	/* Load r6-r7 from r3 src + r12 offset     */
	DLMZBDOT(11,6,7)        /* Look for NULL in these 8 bytes          */
        add     r12,r12,r11     /* Bump the offset from the start          */
	bne-	found		/* If NULL found, done; else keep looking  */

qwbndry:
	addi 	r5,0,16		/* Byte count of 16 into XER for 'load     */
				/* string' instruction later		   */	
loop:		                /* New 16 bytes, look for NULL             */

        mtxer   r5              /* Load count for 'load string'            */

	lswx 	r6,r3,r12	/* Load r6-r9 from r3 src + r12 offset     */
                                /* from the start                          */

	DLMZBDOT(11,6,7)        /* Look for NULL in these current 8 bytes  */
        
        add     r12,r12,r11     /* Bump the offset from the start          */      

	bne-	found		/* If no NULL in first 8 bytes, fall       */
				/* through and test next 8 bytes, else,    */
				/* continue with strrchr function          */ 

        DLMZBDOT(11,8,9)        /* Look for NULL in these current 8 bytes  */

        add     r12,r12,r11     /* Bump the offset from the start          */

	beq+	loop       	/* If no NULL, grab next 16 bytes, else    */
				/* continue with strrchr function          */

found:				/* r12 here has string length plus one     */

	addi	r12,r12,-1      /* Remove NULL to get real string length   */
        
        rlwimi	r4,r4,8,16,23	/* Set up 'c' in all bytes of word.        */
        rlwimi.	r4,r4,16,0,15   /* Remember 'c' in LSB of int              */

        beq-    stringend       /* If search char is NULL, we have answer  */
    
        rlwinm  r5,r4,0,24,31   /* Setup search char in lower byte only    */

        srwi.   r9,r12,3        /* Main loop reads 8 bytes, so how many    */
	                        /* blocks of 8 bytes? If 1 or less,        */ 
        beq-    bbb             /* handle byte-wise.                       */
        
        mtctr   r9              /* Number of 8 byte blocks for loop        */

        add     r8,r3,r12       /* Location of "last" 8 bytes              */
        addi    r8,r8,1         /* Adjust for -8 in next instruction       */

again:

        addi    r8,r8,-8        /* Backup to previous 8 bytes              */

       	lswi 	r6,r8,8 	/* Load r6/r7                              */

       	xor	r6,r6,r4	/* XOR r4 & r6 to look for match           */
	xor	r7,r7,r4	/* XOR r4 & r7 to look for match           */
        
	DLMZBDOT(0,6,7)	        /* Check for hole created by match         */

       	bdnzt-	eq,again        /* If no hole and still more blocks, loop  */
                                /* back                                    */
     
        /* Here, either loop block count went to 0, or match found         */
        
        mr      r7,r8		   /* Save r8 in case we need it later     */
        
        addi    r8,r3,8		   /* Here, add 8 to string start, in case */
        			   /* we do tst from start                 */
        beq     byte_setup         /* If no match, do one byte by byte test*/
                                   /* from the start                       */
                                   
        addi    r8,r7,8		   /* Here, we'll test current 8 bytes     */
        			   /* where a match was found              */
				   
byte_setup:

        li      r11,8		   /* Load CTR reg with 8 for byte loop    */
        
        mtctr   r11        

one_more:			   /* Look for match in 8 (or less) bytes  */
				   /* starting from the end to start.      */
        lbzu    r11,-1(r8)	   /* Load last byte and back up pointer   */

        xor.    r11,r11,r5	   /* Test for match                       */
        beq     done2		   /* If match, we're done, else loop back */
                                   
        bdnz+   one_more 	   /* and test previous byte               */
        
        b       none       	   /* Byte count exhausted, char not found */

done2:  

        mr      r3,r8		   /* Set up required return reg           */
        blr

bbb:				   /* Here if original string length < 9   */

        addi    r12,r12,1
        
        add     r8,r3,r12	   /* Point to end of bytes                */
        
        mtctr   r12		   /* Set up byte count                    */
       
        b       one_more 	   /* And handle in byte-wise loop         */

none:

        xor     r3,r3,r3           /* Return NULL, no match in the string  */
        
        blr
        
stringend:

        add     r3,r3,r12          /* Add string len to start             */
        blr                
        
function_epilog(__strrchr440)
