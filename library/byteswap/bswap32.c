asm("	.text\n\
	.align 2\n\
	.globl bswap32\n\
	.type	bswap32, @function\n\
bswap32:\n\
	rlwinm	%r4,%r3,8,8,31\n\
	rlwimi	%r4,%r3,24,0,7\n\
	rlwimi	%r4,%r3,24,16,23\n\
	or			%r3,%r4,%r4\n\
	blr\n\
");
