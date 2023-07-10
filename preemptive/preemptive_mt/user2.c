#include "types.h"
#include "riscv.h"

__attribute__ ((aligned (16))) char stack0[4096];

uint64 syscall(uint64 nr, uint64 param) {
    uint64 retval;
    asm volatile("add a7, %0, x0" : : "r" (nr));
    asm volatile("add a0, %0, x0" : : "r" (param));
    asm volatile("ecall");
    asm volatile("add %0, a0, x0" : "=r" (retval) );
    return retval;
}

void printhex(uint64 x) {
  int i;
  char s[2];
  s[1] = 0;

  syscall(1, "0x");
  for (i=60; i>=0; i-=4) {
    int d =  ((x >> i) & 0x0f);
    if (d < 10)
      s[0] = d + '0';
    else
      s[0] = d - 10 + 'a';
    syscall(1, s);
  }
  syscall(1, "\n");
}

void foo(void) {
  syscall(1, "Welt ");
}

int main(void) {
    while (1) {
      syscall(1, "2");
      syscall(4, 4);
    }

}

