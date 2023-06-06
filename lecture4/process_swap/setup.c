// For now, the first user program is converted by the Makefile:
// - it is now compiled separately from the kernel
// - the generated ELF file (user1) is converted to binary using objcopy (user1.bin)
// - then, the binary is converted into a byte array "user1_bin" using the xxd tool
// - the byte array is finally included here so it can be used as a source for 
//   copying into the process memory range starting at 0x8010_0000

// TODO: add additional user program(s)
#include "userprogs1.h"

#include "types.h"
#include "riscv.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *);
extern void printhex(uint64);

void copyprog(int process) {
  // TODO: copy user code for given process number to memory at address 0x8010_0000
}

void setup(void) {
  // set M Previous Privilege mode to User so mret returns to user mode.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_U;
  w_mstatus(x);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable software interrupts (ecall) in M mode.
  w_mie(r_mie() | MIE_MSIE);

  // set the machine-mode trap handler to jump to function "ex" when a trap occurs.
  w_mtvec((uint64)ex);

  // disable paging for now.
  w_satp(0);

  // configure Physical Memory Protection to give user mode access to all of physical memory.
  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);

  // copy the first process into memory
  copyprog(1);

  // set M Exception Program Counter to main, for mret, requires gcc -mcmodel=medany
  w_mepc((uint64)0x80100000);

  // switch to user mode (configured in mstatus) and jump to address in mepc CSR -> main().
  asm volatile("mret");
}

