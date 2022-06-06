#include "types.h"
#include "riscv.h"
#include "hardware.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *s);

__attribute__ ((aligned (16))) char kernelstack[4096];

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

// This is the C code part of the exception handler
// "exception" is called from the assembler function "ex" in ex.S with registers saved on the stack
void exception(void) {
  uint64 nr;
  uint64 param;
  uint64 retval = 0;

  // all exceptions end up here - so far, we assume that only syscalls occur
  // will have to decode the reason for the exception later!

  // copy registers a0 and a1 to variables nr and param
  asm volatile("add %0, a7, x0" : "=r" (nr) );
  asm volatile("add %0, a0, x0" : "=r" (param) );

  uint64 pc = r_mepc();

  // decode syscall number
  switch (nr) {
    case 1: printstring((char *)param);
            break;
    case 2: putachar((char)param);
            break;
    case 3: retval = readachar();
            break;
    case 42: // TODO: process exited, switch to the next process
             //       If we are at the last process, start with the first one again
             // Implementation: 
             // 1. call copyprog to load the new process
             // 2. set the pc (for mret) to the correct value
		
             break;
    default: printstring("Invalid syscall: "); printhex(nr); printstring("\n");
             break;
  }

  // adjust return value 
  // we want to return to the instruction _after_ the ecall! (at address mepc+4)
  w_mepc(pc+4);

  // pass the return value back in a0
  asm volatile("add a0, %0, x0" : : "r" (retval)); 

  // this function returns to ex.S
}
