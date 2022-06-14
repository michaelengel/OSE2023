	.section .text
	.global _entry
_entry:
// TODO 3.1: add stack pointer initialization here

	jal	main
goaway:
	li	a7, 42
	li	a0, 0
	ecall

