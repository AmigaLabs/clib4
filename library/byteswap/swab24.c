asm("\
	.text\n\
	.align 8\n\
	.globl swab24\n\
	.type swab24,@function\n\
swab24:\n\
	dcbt		0,%r3\n\
	li			%r7,3\n\
	divwu		%r5,%r5,%r7\n\
	andi.		%r7,%r5,3\n\
	srawi.	%r5,%r5,2\n\
	mtctr		%r5\n\
	or			%r5,%r7,%r7\n\
	li			%r6,0\n\
	bc			4,gt,.postfix\n\
.loop:\n\
	lwbrx		%r8,%r6,%r3\n\
	addi		%r7,%r6,4\n\
	lwzx		%r9,%r7,%r3\n\
	addi		%r7,%r6,8\n\
	lwbrx		%r10,%r7,%r3\n\
	rotlwi	%r8,%r8,8\n\
	or			%r7,%r9,%r9\n\
	rlwimi	%r9,%r8,16,8,15\n\
	rlwimi	%r9,%r10,8,16,23\n\
	rlwimi	%r8,%r7,16,24,31\n\
	rotrwi	%r10,%r10,8\n\
	rlwimi	%r10,%r7,16,0,7\n\
	stwx		%r8,%r6,%r4\n\
	addi		%r6,%r6,4\n\
	stwx		%r9,%r6,%r4\n\
	addi		%r6,%r6,4\n\
	stwx		%r10,%r6,%r4\n\
	addi		%r6,%r6,4\n\
	bc			0,lt,.loop\n\
.postfix:	# Fix any remaining 24-bit words (number of remaining words in r5).\n\
	or.		%r5,%r5,%r5\n\
	bc			4,gt,.exit\n\
	mtctr		%r5\n\
	add		%r3,%r3,%r6\n\
	add		%r6,%r4,%r6\n\
	subi		%r3,%r3,1\n\
.fixloop:\n\
	lbzu		%r7,1(%r3)\n\
	lbzu		%r8,1(%r3)\n\
	lbzu		%r9,1(%r3)\n\
	stb		%r7,2(%r6)\n\
	stb		%r8,1(%r6)\n\
	stb		%r9,0(%r6)\n\
	addi		%r6,%r6,3\n\
	bc			0,lt,.fixloop\n\
.exit:\n\
	or			%r3,%r4,%r4\n\
	blr\n\
");
