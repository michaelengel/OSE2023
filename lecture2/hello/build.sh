#!/bin/bash
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c boot.S
riscv64-unknown-elf-gcc -g -mcmodel=medany -mno-relax -c hello.c
riscv64-unknown-elf-gcc -g -ffreestanding -fno-common -nostdlib -mno-relax -mcmodel=medany   -Wl,-T hello.ld *.o -o hello
riscv64-unknown-elf-objcopy -O binary hello hello.bin
