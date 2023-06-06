#include <stdint.h>
#include "types.h"
#include "riscv.h"
#include "hardware.h"
#include "syscalls.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *s);

__attribute__ ((aligned (16))) char kernelstack[4096];

void printhex(uint64);

volatile struct uart* uart0 = (volatile struct uart *)0x10000000;

// Syscall implementations here (mapping could also use a table as in Unix):

// Syscall 1: printstring. Takes a char *, prints the string to the UART, returns nothing
// Syscall 2: putachar.    Takes a char, prints the character to the UART, returns nothing
// Syscall 3: getachar.    Takes no parameter, reads a character from the UART (keyboard), returns the char

char getachar(void) {
  while ((uart0->LSR & (1<<0)) == 0) ;
  return uart0->RBR;
}

void putachar(char c) {
  while ((uart0->LSR & (1<<5)) == 0) ;
  uart0->THR = c;
}

void printstring(char *s) {
  while (*s) {
    putachar(*s);
    s++;
  }
}

// This is a useful helper function (one you implemented putachar and printstring)
void printhex(uint64 x) {
  int i;
  char s;

  printstring("0x");
  for (i=60; i>=0; i-=4) {
    int d =  ((x >> i) & 0x0f);
    if (d < 10)
      s = d + '0';
    else
      s = d - 10 + 'a';
    putachar(s);
  }
  printstring("\n");
}

// This is the C code part of the exception handler
// "exception" is called from the assembler function "ex" in ex.S with registers saved on the stack

// The stack pointer is passed to the exception function in a0 (first parameter)
// as pointer to the regst struct from ex.S, so the stored register valued on the 
// stack are accessible through the "regs" struct
void exception(struct regs *r) {
  uint64 nr;
  uint64 param;
  uint64 retval = 0;
  uint64 mcause;
  uint64 mepc;

  // The mcause CSR contains information about the cause of the exception
  // We need to read it using an assembler function, since MSRs are not
  // directly accessible from C
  mcause = r_mcause();

  // Copy registers a0 and a1 stores on the stack to variables nr and param
  nr = r->a7;
  param = r->a0;

#if 0
  // Debug output if you see strange exceptions
  printstring("MCAUSE: "); printhex(mcause); printstring("\n");  
  printstring("NR: "); printhex(nr); printstring("\n");  
  printstring("PARAM: "); printhex(param); printstring("\n");  
#endif

  // All exceptions end up here - so far, we assume that only syscalls occur
  // will have to decode the reason for the exception later!

  // The most significant bit (63 on RISC V-64) indicates if the exception
  // cause was synchronous (bit == 0, caused by an instruction execution such
  // as ecall, division by zero, illegal memory access...) or asynchronous
  // (bit == 1, device or timer interrupts)

  // The remaining 63 bits (in fact, the 8 LSB only) indicate the detailed
  // cause (different values for sync/async causes)
  // See "The RISC-V Instruction Set Manual Volume II: Privileged Architecture Document"
  // Version 20211203, // Section 3.1.15 Machine Cause Register (mcause) for details
  // https://github.com/riscv/riscv-isa-manual/releases/download/Priv-v1.12/riscv-privileged-20211203.pdf
  if ((mcause & ((uint64_t)1<<63)) == 0) { // synchronous exception
    if ((mcause & 0xff) == 8) { // Code 8 indicates an ECALL
      switch (nr) { // Now decode the syscall number and call the appropriate function
        case SYS_PRINTSTRING: 
          printstring((char *)param);
          break;
        case SYS_PUTACHAR: 
          putachar((char)param);
          break;
        case SYS_GETACHAR: 
          retval = getachar();
          break;
        case SYS_EXIT: // TODO: process exited, switch to the next process
                       //       If we are at the last process, start with the first one again
                       // Implementation: 
                       // 1. call copyprog to load the new process
                       // 2. set the pc (for mret) to the correct value
          break;
        default: 
          printstring("Unknown Syscall "); printhex(nr); printstring("\n");
          break;
      }
    } else {
      printstring("Unknown Exception "); printhex(mcause); printstring(" at "); 
      // The mepc CSR contains the location of the instruction which was
      // executed when the instruction occured
      printhex(r_mepc()); printstring("\n");
    }
  }

  // Here, we adjust return value - we want to return to the instruction _after_ the ecall! (at address mepc+4)
  // This has to be adapted later for different exception types
  uint64 pc = r_mepc();
  w_mepc(pc+4);

  // Pass the return value back in a0 via the regs struct
  r->a0 = retval;

  // this function returns to ex.S
}
