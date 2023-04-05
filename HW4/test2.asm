	auipc x8, 0x10000
	addi x8, x8, 0
	addi x21, x0, 5
	addi x18, x0, 1
	sw x18, 0, x8
	sw x18, 4, x8
	addi x9, x21, -2
	lw x19 0(x8)
	lw x20 4(x8)
	add x18, x19, x20
	sw x18, 8(x8)
	addi x8, x8, 4
	addi x9, x9, -1
	blt x0, x9, -24
