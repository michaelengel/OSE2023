#include "types.h"

__attribute__ ((aligned (16))) char stack0[4096];

uint64 __attribute__ ((section (".usertext"))) syscall(uint64 nr, uint64 param);

uint64 syscall(uint64 nr, uint64 param) {
    uint64 retval;
    asm volatile("add a7, %0, x0" : : "r" (nr));
    asm volatile("add a0, %0, x0" : : "r" (param));
    asm volatile("ecall");
    asm volatile("add %0, a0, x0" : "=r" (retval) );
    return retval;
}

int main(void) {
#if 0
    volatile int *p = (volatile int *) 0x10000000;
    *p = 42;
#endif

    while (1) {
      syscall(1, "1");
      syscall(4, 8);
    }
    return 0;
}

