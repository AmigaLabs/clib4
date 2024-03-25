asm("\
	.text\n\
	.align 8\n\
	.globl bswap16\n\
	.type	bswap16, @function\n\
bswap16:\n\
#	rlwinm	%r4,%r3,8,16,24\n\
#	rlwimi	%r4,%r3,24,24,31\n\
#	or			%r3,%r4,%r4\n\
	rlwimi	%r3,%r3,16,8,15\n\
	srwi		%r3,%r3,8\n\
	blr\n\
");
