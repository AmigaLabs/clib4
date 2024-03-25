
asm("	.text\n\
	.align 8\n\
	.globl bswap64\n\
	.type	bswap64, @function\n\
bswap64:\n\
	rlwinm	%r5,%r3,8,8,31\n\
	rlwimi	%r5,%r3,24,0,7\n\
	rlwimi	%r5,%r3,24,16,23\n\
	rlwinm	%r3,%r4,8,8,31\n\
	rlwimi	%r3,%r4,24,0,7\n\
	rlwimi	%r3,%r4,24,16,23\n\
	or			%r4,%r5,%r5\n\
	blr\n\
");


