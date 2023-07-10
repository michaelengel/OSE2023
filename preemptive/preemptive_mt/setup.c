#include "userprogs1.h"
#include "userprogs2.h"
#include "userprogs3.h"
#include "types.h"
#include "riscv.h"
#include "hardware.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *);
extern void printhex(uint64);
extern pcb_struct pcb[NPROCS];

void copyin(int process, uint64 address) {
  // copy user code to memory inefficiently... :)
  unsigned char* from;
  int user_bin_len;
  switch (process) {
    case 0: from = (unsigned char *)&user1_bin; user_bin_len = user1_bin_len; break;
    case 1: from = (unsigned char *)&user2_bin; user_bin_len = user2_bin_len; break;
    case 2: from = (unsigned char *)&user3_bin; user_bin_len = user3_bin_len; break;
    default: printstring("unknown process!\n"); printhex(process); printstring("\n"); break;
  }

  unsigned char* to   = (unsigned char *)address;
  // printstring("copy from "); printhex(from); printstring("to "); printhex(to); printstring("len "); printhex(user_bin_len); 
  for (int i=0; i<user_bin_len; i++) {
    *to++ = *from++;
  }
}
 
void copyprog(int process) {
  // copy user code to memory inefficiently... :)
  unsigned char* from;
  int user_bin_len;
  switch (process) {
    case 0: from = (unsigned char *)&user1_bin; user_bin_len = user1_bin_len; break;
    case 1: from = (unsigned char *)&user2_bin; user_bin_len = user2_bin_len; break;
    case 2: from = (unsigned char *)&user3_bin; user_bin_len = user3_bin_len; break;
    default: printstring("unknown process!\n"); printhex(process); printstring("\n"); break;
  }

  unsigned char* to   = (unsigned char *)0x80100000;
  for (int i=0; i<user_bin_len; i++) {
    *to++ = *from++;
  }
}

void plicinit(void) {
  // set desired IRQ priorities non-zero (otherwise disabled). 
  *(uint32*)(PLIC + UART0_IRQ*4) = 1;
    
  // set uart's enable bit for this hart's M-mode.
  *(uint32*)PLIC_MENABLE = (1 << UART0_IRQ);

  // set this hart's M-mode priority threshold to 0.
  *(uint32*)PLIC_MPRIORITY = 0; 

  w_mie(r_mie() | MIE_MEIE);
} 

void timerinit(void) {
  // we only have one CPU...
  int id = 0;

  // ask the CLINT for a timer interrupt.
  int interval = 1000000; // cycles; about 1/10th second in qemu.
  *(uint64*)CLINT_MTIMECMP(id) = *(uint64*)CLINT_MTIME + interval;

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
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
#if 0
  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);            // full access
#endif
  w_pmpaddr0(0x80000000ull >> 2); // 0x0000_0000...0x7fff_ffff
  w_pmpaddr1(0x80100000ull >> 2); // 0x8000_0000...0x800f_ffff
  w_pmpaddr2(0x80200000ull >> 2); // 0x8010_0000...0x801f_ffff
  w_pmpaddr3(0x80300000ull >> 2); // 0x8020_0000...0x802f_ffff
  w_pmpaddr4(0x80400000ull >> 2); // 0x8030_0000...0x803f_ffff
  w_pmpcfg0(0x0f0000);            // 2 - full access 1,0 - no access

  copyin(0, 0x80100000);
  copyin(1, 0x80200000);
  copyin(2, 0x80300000);

  // set M Exception Program Counter to main, for mret, requires gcc -mcmodel=medany
  w_mepc((uint64)0x80100000);

  // init process table
  for (int i=0; i<NPROCS; i++) { 
    pcb[i].pc = 0x80000000ull + 0x100000ull * (i+1);     // process i starts at address 0x80i0_0000
    pcb[i].sp = 0x80000000ull + 0x200000ull * (i+1) - 4; // top of address range, replaced later
    if (i < 3) {
      pcb[i].state = READY;
      pcb[i].pmp   = 0x0f0000ull << (8*i); // access to range in pmpaddr_i CSR
    } else {
      pcb[i].state = INACTIVE;
      pcb[i].pmp   = 0x0; // no access
    }
  }

  timerinit();

  // enable uart rx irqs
  extern volatile struct uart* uart0;
  uart0->IER=0x1;

  plicinit();

  // switch to user mode (configured in mstatus) and jump to address in mepc CSR -> main().
  asm volatile("mret");
}

