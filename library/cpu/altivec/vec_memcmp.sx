//#define __MWERKS__
//------------------------------------------------------------------
// file:  vec_memcmp.S
//    AltiVec enabled version of memcmp
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
// extern int vec_memcmp(const void *ptr1, const void *ptr2, size_t len);
// Returns:
//  value < 0  if ptr1[0:len] <  ptr2[0:len]
//  value = 0  if ptr1[0:len] == ptr2[0:len]
//  value > 0  if ptr1[0:len] >  ptr2[0:len]
//------------------------------------------------------------------

// Revision History:
//    Rev 0.0	Original                          Chuck Corley	05/27/03


#define VRSV 256	//	VRSAVE spr
// Don't use vectors for BC <= MIN_VEC. Works only if MIN_VEC >= 16 bytes.
#define MIN_VEC 16

	// Macros for bits in CR6
#define	_all 6*4+0
#define	_none 6*4+2
	// Macros for condition to be true/false and unlikely/likely to be taken    
#define	_F_u 4
#define	_T_u 12
#define	_T_l 13

// Register useage
#define Rt r0	// 	r0 when used as a temporary register	

#define PT1 r3	// 	entering: ptr1; exiting: return value

#define SRC r4	// 	entering: ptr2; then ptr2+16 if ptr1[28:31]<ptr2[28:31]

#define BC r5	//	entering: Byte_Count
#define BCM1 r5	//	then Byte_Count -1

#define DMS r6	//      	ptr1 - ptr2 initially
#define S2 r6	//  	ptr2 bytes initially

// Codewarrior will put an unwelcome space as "lbzu	r0,1(r7 )"
// if you don't put the comment right after the r7.  CJC 030314
#define SM1 r7//	ptr2 -1 for byte-by-byte forwards initially
#define S r7	//	ptr2[28:31]
#define BK r7	//	byte index

#define DM1 r8//	ptr1 -1 for byte-by-byte forwards initially
#define D r8	//	ptr1[28:31]

#define PBC r9	//	ptr1 + byte count initially

#define S1 r10	//  	ptr1 bytes initially
#define PBK r10	//	(ptr1+byte_count-1)

#define DR r11	//	(ptr1+16)[0:27]
#define QW r11	//  	number of quad words (vectors)

#define RSV r12	//  	storage for VRSAVE register if used

#define VS1 v0	// 	source 1 as a vector of 16 bytes

#define VS2 v1	// 	source 2 as a vector of 16 bytes

#define VS2b v2	//  	second source 2 vector for permuting
#define VS12B v2	// 	octet shift count of 12
#define VMB3 v2	// 	mismatch shifted right 3 bytes

#define VP1 v3	// 	source 1 permute register
#define VSH16 v3	// 	octet shift count of 16 bits/2 octets
#define VMW3 v3	// 	mismatch shifted right 3 words

#define VS1B v4	// 	octet shift count of 1

#define V0 v5	// 	all zeros

#define VP2 v6	// 	source 2 permute register
#define VS4B v6	// 	octet shift count of 4
#define VMB1 v6	// 	mismatch shifted right one byte

#define VSH1 v7	// 	shift count of 1 bit
#define VS8B v7	// 	octet shift count of 8 octets
#define VMB2 v7	// 	mismatch shifted right 2 bytes

#define V1 v8	// 	all ones
#define VCE v8	// 	equality compare destination register

#define VS2a v9	//  	first source 2 vector for permuting
#define VMW1 v9	// 	mismatch shifted right one word

#define VP3 v10	// 	ptr1-ptr2 permute register
#define VMW2 v10	// 	mismatch shifted right 2 words

#define VM v11	// 	mask for right end of 1st S1 vector

#define VP4   v12	//	last mask permute vector
#define VLM   v12	//      last mask register

#define VMM   v13	// 	vector of zeroes with ones at mismatch(es) and DN

//  Condition register use
//      cr0[0:2] = (ptr1-ptr2==0)? return
// then cr0[0:2] = (ptr1[28:31]-ptr2[28:31]<0)? "Need more S2?";
//      cr1[0,2] = (BC == 0)? 1 : 0; (nothing to move)
// then cr1[2]   = (QW == 0)? 1 : 0; (Any full vectors to move?)
//      cr5[2]   = ((PBK = PT1+BC)[28:31] = 0)? 1 : 0; (S1N right justified)
//      cr6[0]   = (S1 == S2)?1:0;  (By vector)
// then cr6[2]   = (S2 > S1)? 1 : 0; (At mismatched byte)
//      cr7[2]   = (BC>MIN_VEC)?1:0;  (BC big enough to warrant vectors)

	.text
#ifdef __MWERKS__
	.align	32
#else
	.align	5
#endif

#ifdef LIBMOTOVEC
	.global	memcmp     
memcmp:
#else
	.global	vec_memcmp     
vec_memcmp:
#endif
	subf.	DMS,SRC,PT1	// IU1 Compute ptr1-ptr2 difference
	cmpi	cr1,0,BC,0	// IU1 Eliminate zero byte count moves
	cmpi	cr7,0,BC,MIN_VEC	// IU1 Check for minimum byte count

	add	PBC,PT1,BC	// IU1 Address of last byte + 1
	addi	SM1,SRC,-1	// IU1 Pre-bias and duplicate ptr2
	addi	DR,PT1,16		// IU1 Duplicate s1 pointer
	beq	Dumb_exit		// return if PT1 = SRC

	addi	PBK,PBC,-1	// IU1 Address of last ptr1 byte
	addi	DM1,PT1,-1	// IU1 Pre-bias and duplicate ptr1
	rlwinm	DR,DR,0,0,27	// IU1 (PT1+16)[0:27]
	beq	cr1,Dumb_exit	// return if BC = 0

	subf	QW,DR,PBK		// IU1 Bytes of full vectors to move (-16)
	rlwinm	PBC,PBC,0,28,31
	bgt	cr7,v_memcmp	// do as vectors if BC>MIN_VEC
	
// Compare byte-by-byte if BC<=MIN_VEC	
	mtctr	BC		// i=BC; do if...;i--; while (i>0)
Cmp_nxt_byte:
	lbzu	S2,1(SM1)		// LSU
	lbzu	S1,1(DM1)		// LSU
	subf.	PT1,S2,S1		// IU1 if (*s1++ == *s2++)
	bdnzt	2,Cmp_nxt_byte	// b while equal and bytes left
	blr
Dumb_exit:
	xor	PT1,PT1,PT1	// IU1 return zero
	blr

#ifdef __MWERKS__
	.align	16
#else
	.align	4
#endif
v_memcmp:
// Byte count < MIN_VEC bytes will have been compared by scalar code above,
// so this will not deal with small block compares < MIN_VEC.

#ifdef VRSAVE
	mfspr	RSV,VRSV		// IU2 Get current VRSAVE contents
#endif
	rlwinm	QW,QW,28,4,31	// IU1 Quad words remaining
	rlwinm	S,SRC,0,28,31	// IU1 Save ptr2 address bits s[28:31]

#ifdef VRSAVE
	oris	Rt,RSV,0xfff8	// IU1 Or in registers used by this routine
#endif	
	rlwinm	D,PT1,0,28,31	// IU1 D = ptr1[28:31]
	cmpi	cr1,0,QW,0	// IU1 Any full vectors to move?

#ifdef VRSAVE
	mtspr	VRSV,Rt		// IU2 Save in VRSAVE before first vec op
#endif
	lvxl	VS1,0,PT1		// LSU Get source1 load started (load as LRU)
	subf.	S,S,D		// IU1 Is s2 longer than s1? (28:31 greater?)
	li	BK,16		// IU1 Byte index pointer

	lvxl	VS2,0,SRC		// LSU Get source2 load started (load as LRU)
	vor	VS2b,VS2,VS2	// VIU1 Preset second s2 vector if not loaded
	addi	BCM1,BC,-1	// IU1 Index to last s2 byte
// Decide if second vector of S2 is needed to compare to first vector of S1
	bge	Around		// b if initial S1 is shorter than or equal S2

	lvxl	VS2b,SRC,BK	// LSU Otherwise, we need more of s2
	addi	SRC,SRC,16	// IU1 Increment s2 pointer
	addi	BCM1,BCM1,-16	// IU1 Correction for last byte
Around:

	lvsl	VP1,0,PT1		// LSU Set permute vector for s1 shift left
	vspltisb	VS1B,8		// VPU Create a shift count for 1 octet/8 bits
	vxor	V0,V0,V0		// VIU1 Create a vector of all zeroes

	lvsl	VP2,0,SRC		// LSU Set permute vector for s2 shift left
	vspltisb	VSH1,1		// VPU Create a shift count of 1 bit
	vnor	V1,V0,V0		// VIU1 Create a vector of all ones

	lvsr	VP3,0,DMS		// LSU Set permute vector for S2-S1 difference
	cmpi	cr5,0,PBC,0	// IU1 Will last byte of S2 be rt justified?
	vperm	VM,V0,V1,VP1	// VPU Mask as long as our subset of 1.


	lvsr	VP4,0,PBC		// VIU1 Permute vector for bytes rt of end
// Dealing with first S1 Vector - Permute S1 and S2 (possibly + S2b) to left edge
	vperm	VS1,VS1,V1,VP1	// VPU Left align s1 with ones as pad

	vperm	VS2,VS2,VS2b,VP2	// VPU Left align s2 and s2+

	vslb	VSH16,VS1B,VSH1	// VPU Shift count for 16 bits/2 octets
	vor	VS2,VS2,VM	// VIU1 s2 now has identical ones padding to s1

	vslb	VS4B,VSH16,VSH1	// VPU Create a shift count for 4 octets
	vcmpequb.	VCE,VS1,VS2	// VIU1 Does s1 = s2?

	vslb	VS8B,VS4B,VSH1	// VPU Create a shift count for 8 octets
	vnor	VMM,VCE,VCE	// VIU1 Not equals become ones
	bc	_F_u,_all,memcmp_final_v_NE	// b if s1!=s2

	ble	cr1,Last_ld	// b if there are no QW to do
	mtctr	QW		// IU2 i=QW; do ...;  while (i-- > 0)

// Dealing with middle vectors
memcmp_NA_next_v:
	lvxl	VS2a,SRC,BK	// LSU Get next 16 bytes of s2

	lvxl	VS1,PT1,BK	// LSU Get next 16 bytes of s1
	addi	BK,BK,16		// IU1 Increment byte index

	vperm	VS2,VS2b,VS2a,VP3	// VPU Combine into left justified s2
	vor	VS2b,VS2a,VS2a	// VIU1 Move upper vector to lower

	vcmpequb.	VCE,VS1,VS2	// VIU1 Does s1 == s2 ?

	vnor	VMM,VCE,VCE	// VIU1 Not equals become ones
	bdnzt	24,memcmp_NA_next_v	// b if more whole QWs to do and s1==s2

	bc	_F_u,_all,memcmp_final_v_NE	// b if s1 != s2
	
// Dealing with last vector
Last_ld:
	lvxl	VS2a,SRC,BCM1	// LSU Last load of s2 (perhaps redundant)
	vperm	VLM,V0,V1,VP4	// VPU Ones mask for bytes rt of end

	lvxl	VS1,PT1,BK	// LSU Last load of s1

	vperm	VS2,VS2b,VS2a,VP3	// VPU Combine into left justified s2
	beq	cr5,Rt_just	// b if final S1 byte is rt justified

	vor	VS2,VS2,VLM	// VIU1 Set uninvolved bytes at end

	vor	VS1,VS1,VLM	// VIU1 Set bytes at end of s1
Rt_just:
	vcmpequb.	VCE,VS1,VS2	// VIU1 Does s1 == s2 ?

	vnor	VMM,VCE,VCE	// VIU1 Not equals become ones
	bc	_F_u,_all,memcmp_final_v_NE	// b if s1!=s2

	xor	PT1,PT1,PT1	// IU1 Will return zero if strings are equal
#ifdef VRSAVE
	mtspr	VRSV,RSV	// IU1 Restore VRSAVE	
#endif
	blr			// Return 0 if s1 == s2	
	
memcmp_final_v_NE:
	// s1 != s2, We're going to create a mask to mask off everything to
	// the right of the first mismatching byte so we know we are just
	// looking at the string up to the mismatch.

	vsum4ubs	VS12B,VS1B,VS8B	// VIU2 Create a shift count for 12 octets

	vsro	VMW1,VMM,VS4B	// VPU Shift the compare result one word right
	vsrw	VMB1,VMM,VS1B	// VIU1 Shift compare result 8 bits right
	
	vsro	VMW2,VMM,VS8B	// VPU Shift the compare result 2 words right
	vsrw	VMB2,VMM,VSH16	// VIU1 Shift compare result 16 bits right

	vsro	VMW3,VMM,VS12B	// VPU Shift the compare result 3 words right
	vor	VM,VMW1,VMW2	// VIU1 Mask of words one and 2 to the right
	
	vsro	VMB3,VMB2,VS1B	// VPU Shift compare result 3 bytes right
	vor	VM,VM,VMW3	// VIU1 Mask of MM 1,2,&3 words to the right

	vcmpgtuw	VM,VM,V0		// VIU1 Mask of all ones in words to the right
	
	vor	VM,VM,VMB1	// VIU1 Or in first byte to right

	vor	VM,VM,VMB2	// VIU1 Or in second byte to right

	vor	VM,VM,VMB3	// VIU1 Or in third byte to right

	vor	VS2,VS2,VM	// VIU1 Set bytes right of mismatch

	vor	VS1,VS1,VM	// VIU1 Set bytes right of mismatch
	li	r3,-1		// IU1 Return -1 if s1 < s2	

	vcmpgtub.	VCE,VS2,VS1	// VIU1 Compute s2 > s1 for all bytes
#ifdef VRSAVE
	mtspr	VRSV,RSV	// IU1 Restore VRSAVE	
#endif
	bclr	_F_u,_none	// s1 < s2 in first byte with a mismatch		
		
S2_lt_S1:	li	r3,1		// IU1 Return +1 if s1 > s2
	blr	// s1 > s2 in first byte with a mismatch	

// End of memcmp in AltiVec

