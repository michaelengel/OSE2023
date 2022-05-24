#include "types.h"

uint64 syscall(uint64 nr, uint64 param) {
    uint64 retval;

    // TODO: add inline assembler code to copy the value in variable nr to register a7

    // here's our ecall!
    asm volatile("ecall");

    // TODO:add inline assembler code to copy the return value in register a0 to variable retval

    // Here we return the return value...
    return retval;
}

int main(void) {
    char c = 0;
    syscall(0, (uint64)"Hallo Bamberg!\n");
    do {
      c = syscall(2, 0);
      if (c >= 'a' && c <= 'z') c = c & ~0x20;
      syscall(1, c);
    } while (c != 'X');

    syscall(0, (uint64)"This is the end!\n");
    return 0;
}

