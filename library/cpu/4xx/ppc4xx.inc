/* include/ppc4xx.inc, pl_common, pl_linux 12/11/03 13:43:26
 *-----------------------------------------------------------------------------
 *
 *  Copyright (C) 2003 IBM Corporation
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
 */

#ifndef ppc4xx_inc
#define ppc4xx_inc


/*----------------------------------------------------------------------------+
| Function prolog definition and other GCC/EABI defines.
+----------------------------------------------------------------------------*/

#define r0 %r0
#define r1 %r1
#define r2 %r2
#define r3 %r3
#define r4 %r4
#define r5 %r5
#define r6 %r6
#define r7 %r7
#define r8 %r8
#define r9 %r9
#define r10 %r10
#define r11 %r11
#define r12 %r12
#define r13 %r13
#define r14 %r14
#define r15 %r15
#define r16 %r16
#define r17 %r17
#define r18 %r18
#define r19 %r19
#define r20 %r20
#define r21 %r21
#define r22 %r22
#define r23 %r23
#define r24 %r24
#define r25 %r25
#define r26 %r26
#define r27 %r27
#define r28 %r28
#define r29 %r29
#define r30 %r30
#define r31 %r31

#define cr0	%cr0
#define cr1	%cr1
#define cr2	%cr2
#define cr3	%cr3
#define cr4	%cr4
#define cr5	%cr5
#define cr6	%cr6
#define cr7	%cr7

/* helpful cr bit definitions */         
#define cr0_0   0
#define cr0_1   1
#define cr0_2   2
#define cr0_3   3

#define cr1_0   4
#define cr1_1   5
#define cr1_2   6
#define cr1_3   7

#define cr2_0   8
#define cr2_1   9
#define cr2_2   10
#define cr2_3   11

#define cr3_0   12
#define cr3_1   13
#define cr3_2   14
#define cr3_3   15

#define cr4_0   16
#define cr4_1   17
#define cr4_2   18
#define cr4_3   19

#define cr5_0   20
#define cr5_1   21
#define cr5_2   22
#define cr5_3   23

#define cr6_0   24
#define cr6_1   25
#define cr6_2   26
#define cr6_3   27

#define cr7_0   28
#define cr7_1   29
#define cr7_2   30
#define cr7_3   31


#define function_prolog(func_name) \
					.text; \
					.align  2; \
					.globl  func_name; \
					func_name:

#define function_prolog_alignedto(func_name, xalign, xwords) \
  					.text; \
					.globl  func_name; \
					.align  xalign; \
					EALIGN_W_##xwords; \
					func_name:

#define function_epilog(func_name)	\
					.type func_name,@function; \
					.size func_name,.-func_name

#define function_call(func_name)	bl func_name 

#define EALIGN_W_0  /* No words to insert.  */
#define EALIGN_W_1  nop
#define EALIGN_W_2  nop;nop
#define EALIGN_W_3  nop;nop;nop
#define EALIGN_W_4  EALIGN_W_3;nop
#define EALIGN_W_5  EALIGN_W_4;nop
#define EALIGN_W_6  EALIGN_W_5;nop
#define EALIGN_W_7  EALIGN_W_6;nop

#define stringify_in_c(...)   __VA_ARGS__
#define ASM_CONST(x)      x

#define PPC_LL      stringify_in_c(lwz)
#define PPC_STL     stringify_in_c(stw)
#define PPC_STLU    stringify_in_c(stwu)
#define PPC_LCMPI   stringify_in_c(cmpwi)
#define PPC_LONG    stringify_in_c(.long)
#define PPC_LONG_ALIGN  stringify_in_c(.balign 4)
#define PPC_TLNEI   stringify_in_c(twnei)
#define PPC_LLARX(t, a, b, eh)  PPC_LWARX(t, a, b, eh)
#define PPC_STLCX   stringify_in_c(stwcx.)
#define PPC_CNTLZL  stringify_in_c(cntlzw)
#define PPC_MTOCRF  stringify_in_c(mtcrf)
#define PPC_LR_STKOFF   4
#define PPC_MIN_STKFRM  16

#endif
