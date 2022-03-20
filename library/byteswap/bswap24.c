asm("	.text\n\
	.align 2\n\
	.globl bswap24\n\
	.type	bswap24, @function\n\
bswap24:\n\
	rlwinm	%r4,%r3,16,8,31\n\
	rlwimi	%r4,%r3,0,16,24\n\
	or			%r3,%r4,%r4\n\
	blr\n\
");
