	.global sigsetjmp
	.global __sigsetjmp
	.type sigsetjmp,%function
	.type __sigsetjmp,%function
sigsetjmp:
__sigsetjmp:
	cmpwi cr7, 4, 0
	beq- cr7, 1f

	mflr 5
	stw 5, 448(3)
	stw 16, 448+4+8(3)
	mr 16, 3

.hidden setjmp
	bl setjmp

	mr 4, 3
	mr 3, 16
	lwz 5, 448(3)
	mtlr 5
	lwz 16, 448+4+8(3)

1:	b setjmp