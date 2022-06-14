#include "types.h"

// TODO 3.1: add a data structure for the user stack

uint64 syscall(uint64 nr, uint64 param) {
    uint64 retval;
    asm volatile("mv a7, %0" : : "r" (nr));
    asm volatile("mv a0, %0" : : "r" (param));
    asm volatile("ecall");
    asm volatile("mv %0, a0" : "=r" (retval) );
    return retval;
}

int main(void) {
    syscall(2, 'b');
    syscall(42, 0);
}
