   asm(".section	\".text\"             \n\
	.align 2                          \n\
	.globl __Clib2Call                \n\
	.type	__Clib2Call, @function      \n\
__Clib2Call:                            \n\
	lis %r11, IClib2@ha               \n\
	lwz %r0, IClib2@l(%r11)           \n\
	lwzx %r11,%r12,%r0                \n\
	mtctr %r11                        \n\
	bctr                              \n\
	.size	__Clib2Call, .-__Clib2Call");
