//------------------------------------------------------------------
// file:  vec_strcpy.S
//    AltiVec enabled version of strcpy and strncpy
//------------------------------------------------------------------

//------------------------------------------------------------------
//	Copyright Motorola, Inc. 2003
//	ALL RIGHTS RESERVED
//
//	You are hereby granted a copyright license to use, modify, and 
//	distribute the SOFTWARE so long as this entire notice is retained 
//	without alteration in any modified and/or redistributed versions, 
//	and that such modified versions are clearly identified as such.  
//	No licenses are granted by implication, estoppel or otherwise under 
//	any patents or trademarks of Motorola, Inc.
//
//	The SOFTWARE is provided on an "AS IS" basis and without warranty.  
//	To the maximum extent permitted by applicable law, MOTOROLA DISCLAIMS 
//	ALL WARRANTIES WHETHER EXPRESS OR IMPLIED, INCLUDING IMPLIED 
//	WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR 
//	PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH 
//	REGARD TO THE SOFTWARE (INCLUDING ANY MODIFIED VERSIONS 
//	THEREOF) AND ANY ACCOMPANYING WRITTEN MATERIALS. 
//
//	To the maximum extent permitted by applicable law, IN NO EVENT SHALL 
//	MOTOROLA BE LIABLE FOR ANY DAMAGES WHATSOEVER 
//	(INCLUDING WITHOUT LIMITATION, DAMAGES FOR LOSS OF 
//	BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS 
//	INFORMATION, OR OTHER PECUNIARY LOSS) ARISING OF THE USE OR 
//	INABILITY TO USE THE SOFTWARE.   Motorola assumes no responsibility 
//	for the maintenance and support of the SOFTWARE.
//------------------------------------------------------------------

//------------------------------------------------------------------
// extern  char *vec_strcpy(char *dest, const char *src);
//           
// Returns:
//  char *dest
//------------------------------------------------------------------

// Revision History:
//    Rev 0.0	Original                          Chuck Corley	03/22/02
//    Rev 0.1   Modified per vec_memcpy rev 0.30  Chuck Corley  05/24/03
//

// Harbison and Steele says "the results of both strcpy, strncpy, ... are
// unpredictable if the two string arguments overlap in memory."
// Since we do not know the address of the end of the string, copying
// from back to front is not an option.  Therefore we always "copy forward."

#define VRSV 256	//	VRSAVE spr
// Use scalar for first MIN_SCALAR bytes. Overhead for vector is too great to win.
#define MIN_SCALAR 32
// Also don't use vectors if |DST-SRC| <= MIN_VEC. Works only if MIN_VEC >= 16 bytes.
#define MIN_VEC 16
#define PAGE_SIZE 4096	// True for G4 with AltiVec

// Register useage:
#define Rt r0	// 	r0 when used as a temporary register	

#define DST r3	// 	entering: dst pointer; exiting: same dst pointer

#define SRC r4	// 	entering: src ptr; then end of src range index (SRC+BC) in memmove

#define ADD r5	//      Temporary future dst address
#define PBC r5	//	Computed Byte_Count to next 4K page src boundary

#define DMS r6	//      dst - src initially

#define SMD r7	//      src - dst initially

#define DD r8	//	duplicate of dst register for incementing

#define QBC r9	// 	Computed Byte_Count to next QW dst boundary

#define DS r10	//	duplicate of src register for speculative incementing

#define PSZ r11	//	storage for page size constant

#define RSV r12	//  	storage for VRSAVE register if used

#define V0    v0	// 	all zeros

#define VS0   v1	//  	src vector for permuting

#define VS1   v2	//  	src vector for permuting

#define VS2   v3	//  	src vector for permuting

#define VP3   v4	// 	alignment permute register

#define VPS0  v5	// 	permuted source vector to store

#define VPS1  v6	//  	2nd permuted source vector to store

#define VCN  v7		//  	null comparison result register

// Conditionalize the use of dcba.  It will help if the data is
// not in cache and hurt if it is.  Generally, except for small
// benchmarks repeated many times, we assume data is not in cache
// (data streaming) and using dcbz is a performance boost.
#ifndef NO_DCBA
#if defined(__GNUC__) || defined(__MWERKS__) || defined(_DIAB_TOOL)
 // gcc and codewarrior and diab don't assemble dcba
#define DCBA .long 0x7c0045ec
// dcba 0,r8     or    dcba 0,DD
#else
#ifdef __ghs__
.macro DCBA
.long 7c0045ec
.endm
#else
#define DCBA dcba 0,DD
#endif  // __ghs__
#endif  // __GNUC__ or __MWERKS__
#else
#define DCBA nop
#endif  // NO_DCBA

	.text
#ifdef __MWERKS__
	.align	32
#else
	.align	5
#endif

#ifdef LIBMOTOVEC
	.global	strcpy     
strcpy:
#else
	.global	vec_strcpy     
vec_strcpy:
#endif


	addi	ADD,DST,32	// IU1 Next dst cacheline
	subf.	DMS,SRC,DST	// IU1 Compute dst-src difference
	subf	SMD,DST,SRC	// IU1 src-dst for use if dst-src<0

	rlwinm	ADD,ADD,0,0,26	// IU1 Round down to even QW
	mr	DD,DST		// IU1 Duplicate dest
	beqlr			// return if DST = SRC

	bgt	Pos_value	// b if DST-SRC>0
	mr	DMS,SMD		// IU1 |dst - src| = src - dst
Pos_value:
	subf.	QBC,DST,ADD	// IU1 Bytes to even QW start of vect (min 32)
	addi	ADD,DD,PAGE_SIZE	// IU1 dst addr in next 4K page
	cmpi	cr7,0,DMS,MIN_VEC	// IU1 Check for min byte count separation

	mtctr	QBC		// IU2 Init counter
Byte_loop:
	lbzx	Rt,0,SRC		// LSU Get a byte
	addi	SRC,SRC,1		// IU1 Increment src
	
	cmpi	cr1,0,Rt,0	// IU1 Is the byte loaded null?
	stbx	Rt,0,DD		// LSU Store it
	addi	DD,DD,1		// IU1 Increment dest
	bdnzf	6,Byte_loop	// b to get another if this one wasn't null

	beqlr	cr1		// return if found a null
	
	li	PSZ,PAGE_SIZE	// IU1 Constant for potential use in vector
	rlwinm	ADD,ADD,0,0,19	// IU1 First address in next 4K page
	mr	DS,SRC		// IU1 Get current src addr
	ble	cr7,Byte_loop	// do by bytes forever if < MIN_VEC separation
	
v_strcpy:
// For systems using VRSAVE, define VRSAVE=1 when compiling.  For systems
// that don't, make sure VRSAVE is undefined.
#ifdef VRSAVE
	mfspr	RSV,VRSV	// IU2 Get current VRSAVE contents
#endif
	subf.	PBC,DD,ADD	// IU1 Now bytes to next 4K page

#ifdef VRSAVE
	oris	Rt,RSV,0xff00	// IU1 Or in registers used by this routine
#endif	
	rlwinm	PBC,PBC,28,4,31	// IU1 Now QWs to next 4K page

#ifdef VRSAVE
	mtspr	VRSV,Rt		// IU2 Save in VRSAVE before first vec op
#endif
// Since DD has to be QW aligned at this point, we need three (or two 
// if SRC[28:31]==0) source vectors to permute into two dest vectors.
// Loading beyond the end of the string should be okay as long as we don't
// cross a page boundary.

	lvsl	VP3,0,SRC		// LSU Create left permute vector
	vxor	V0,V0,V0		// VIU Clear v0
	ble	New_page_0	// b if next load will cross page boundary
	mtctr	PBC		// IU2 Okay to load up to next page
Page_0:

	lvx	VS0,0,DS		// LSU Get first src vector
	addi	DS,DS,16		// IU1 Increment vector src pointer
	bdz	New_page_1	// b if next load will cross page boundary
Page_1:

	lvx	VS1,0,DS		// LSU Get second src vector
	addi	DS,DS,16		// IU1 Increment vector src pointer
	bdz	New_page_2	// b if next load will cross page boundary
Page_2:	

	lvx	VS2,0,DS	// LSU Get third src vector
	addi	DS,DS,16	// IU1 Increment vector src pointer
	bdz	New_page_3	// b if next load will cross page boundary
Page_3:	
	
	vperm	VPS0,VS0,VS1,VP3	// VPU Align S0 and S1 to D0

	vperm	VPS1,VS1,VS2,VP3	// VPU Align S1 and S2 to D1
	vor	VS0,VS2,VS2	// VIU1 Move upper vector to lower

	vcmpequb.	VCN,V0,VPS0		// VIU1 Check for null
	bne	cr6,Final_0	// b if found a null in this permuted source vector
	addi	SRC,SRC,16	// IU1 Increment byte src pointer
	
	vcmpequb.	VCN,V0,VPS1		// VIU1 Check for null
	bne	cr6,Final_1	// b if found a null in this permuted source vector
	DCBA			// LSU Conditionally dcba 0,DST
	addi	SRC,SRC,16	// IU1 Increment byte src pointer

	stvx	VPS0,0,DD	// LSU Store 16 bytes at dst addr D0
	addi	DD,DD,16	// IU1 Increment duplicate dst pointer

	stvx	VPS1,0,DD	// LSU Store 16 bytes at dst addr D1
	addi	DD,DD,16	// IU1 Increment duplicate dst pointer
	
	b	Page_1
	
Final_1:	// Found a null in 2nd vector, store 1st vector then do bytes
	stvx	VPS0,0,DD	// LSU Store 16 bytes at dst addr D0
	addi	DD,DD,16	// IU1 Increment duplicate dst pointer
	
Final_0:	// Found a null in vector, load and store bytes to null instead	
	lbzx	Rt,0,SRC	// LSU Get a byte
	addi	SRC,SRC,1	// IU1 Increment src
	
	cmpi	cr1,0,Rt,0	// IU1 Is the byte loaded null?
	stbx	Rt,0,DD		// LSU Store it
	addi	DD,DD,1		// IU1 Increment dest

	bne	cr1,Final_0	// b to get another if this one wasn't null
	
#ifdef VRSAVE
	mtspr	VRSV,RSV	// IU1 Restore VRSAVE	
#endif
	blr

New_page_0:	// Next load will be from new page; (ctr would have been <= zero)
	mtctr	PSZ	// reinitialize counter
	b	Page_0

New_page_1:	// Did VS0 contain any nulls?
	vcmpequb.	VCN,V0,VS0		// VIU1 Check for null
	bnl	cr6,Final_0	// b if found a null in this source vector
	mtctr	PSZ	// reinitialize counter
	b	Page_1

New_page_2:	// Did VS1 contain any nulls?
	vcmpequb.	VCN,V0,VS1		// VIU1 Check for null
	bnl	cr6,Final_0	// b if found a null in this source vector
	mtctr	PSZ	// reinitialize counter
	b	Page_2
	
New_page_3:	// Did VS2 contain any nulls?
	vcmpequb.	VCN,V0,VS2		// VIU1 Check for null
	bnl	cr6,Final_0	// b if found a null in this source vector
	mtctr	PSZ	// reinitialize counter
	b	Page_3

// End of strcpy in AltiVec
