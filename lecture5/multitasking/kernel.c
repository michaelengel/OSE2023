#include "types.h"
#include "riscv.h"
#include "hardware.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *s);

__attribute__ ((aligned (16))) char kernelstack[4096];
__attribute__ ((aligned (16))) char userstack[4096];

void printhex(uint64);
void putachar(char);
void copyprog(int);

volatile struct uart* uart0 = (volatile struct uart *)0x10000000;

// our syscalls
void printstring(char *s) {
    while (*s) {     // as long as the character is not null
        putachar(*s); // output the character
        s++;         // and progress to the next character
    }
}

void putachar(char c) {
    while ((uart0->LSR & (1<<5)) == 0)
        ;        // do nothing - wait until bit 5 of LSR = 1
    uart0->THR = c; // then write the character
}

char readachar(void) {
    while ((uart0->LSR & (1<<0)) == 0)
        ;        // do nothing - wait until bit 5 of LSR = 1
    return uart0->RBR; // then read the character
}

// just a helper function
void printhex(uint64 x) {
  int i;
  char s[2];
  s[1] = 0;

  printstring("0x");
  for (i=60; i>=0; i-=4) {
    int d =  ((x >> i) & 0x0f);
    if (d < 10)
      s[0] = d + '0';
    else
      s[0] = d - 10 + 'a';
    printstring(s);
  }
  printstring("\n");
}

int current_process = 0;

// This is the C code part of the exception handler
// "exception" is called from the assembler function "ex" in ex.S with registers saved on the stack
void exception(void) {
  uint64 nr;
  uint64 param;
  uint64 retval = 0;

  // copy registers a0 and a1 to variables nr and param
  asm volatile("mv %0, a7" : "=r" (nr) );
  asm volatile("mv %0, a0" : "=r" (param) );

  // TODO 3.1: Add code to detect exceptions that are not system calls here.
  //           Print the values of the mcause, mepc and mtval registers

  // TODO 3.2: Create a data structure (PCB) for storing the PC and SP of each process.
  //           Save registers when exception is entered - you may need to change ex.S, too!

  // TODO 3.2: Implement a simple round-robin scheduler within the yield system call.

  uint64 pc = r_mepc();

  // decode syscall number

  // TODO 3.2: Add handling of system call 23 (yield) here
  switch (nr) {
    case 1: printstring((char *)param);
            break;
    case 2: putachar((char)param);
            break;
    case 3: retval = readachar();
            break;
    case 42: // printstring("user program returned, starting from the beginning\n");
             current_process++;
             if (current_process > 1) current_process = 0;

             copyprog(current_process);
            
             pc = (uint64)0x80100000-4;
             break;
    default: printstring("Invalid syscall: "); printhex(nr); printstring("\n");
             break;
  }

  // adjust return value - we want to return to the instruction _after_ the ecall! (at address mepc+4)
  w_mepc(pc+4);

  // TODO 3.2: Return the value of the stack pointer so that restoring of the correct registers
  //           in ex.S works...

  // pass the return value back in a0
  asm volatile("mv a0, %0" : : "r" (retval)); 

  // this function returns to ex.S
}
