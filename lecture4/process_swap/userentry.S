	.section .text
	.global _entry
_entry:
	jal	main
loop:
	j	loop

