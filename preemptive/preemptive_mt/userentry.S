	.section .text
	.global _entry
_entry:
        la      sp, stack0
        li      a0, 4096
        add     sp, sp, a0

	jal	main
loop:
	j	loop

