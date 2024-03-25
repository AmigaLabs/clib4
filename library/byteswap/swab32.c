asm("\
	.text\n\
	.align 8\n\
	.globl swab32\n\
	.type swab32,@function\n\
swab32:\n\
	srawi.	%r5,%r5,2\n\
	li			%r6,0\n\
	bc			4,gt,.exit\n\
	mtctr		%r5\n\
.loop:\n\
	lwbrx		%r7,%r6,%r3\n\
	stwx		%r7,%r6,%r4\n\
	addi		%r6,%r6,4\n\
	bc			0,lt,.loop\n\
.exit:\n\
	or			%r3,%r4,%r4\n\
	blr\n\
");


