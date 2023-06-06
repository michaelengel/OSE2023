#include "types.h"
#include "syscalls.h"

uint64 syscall(uint64 nr, uint64 param) {
    uint64 retval;

    // Inline assembler code to copy the value in variable nr to register a7
    asm volatile("mv a7, %0" :: "r"(nr) );
    asm volatile("mv a0, %0" :: "r"(param) );

    // here's our ecall!
    asm volatile("ecall");

    // Inline assembler code to copy the return value in register a0 to variable retval
    asm volatile("mv %0, a0" : "=r"(retval));

    // Here we return the return value...
    return retval;
}

int main(void) {
    char c = 0;
    syscall(SYS_PRINTSTRING, (uint64)"Hallo Bamberg!\n");
    do {
      c = syscall(SYS_GETACHAR, 0);
      if (c >= 'a' && c <= 'z') c = c & ~0x20;
      syscall(SYS_PUTACHAR, c);
    } while (c != 'X');

    syscall(SYS_PRINTSTRING, (uint64)"This is the end!\n");

    // return here would cause a jump to an undefined location - 
    // we did not call main, but jumped to it using mret!
    while(1);
}

