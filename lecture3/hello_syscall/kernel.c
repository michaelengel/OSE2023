#include "types.h"
#include "riscv.h"
#include "hardware.h"

extern int main(void);
extern void ex(void);
extern void printstring(char *s);

__attribute__ ((aligned (16))) char stack0[4096];

void printhex(uint64);

volatile struct uart* uart0 = (volatile struct uart *)0x10000000;

// TODO: implement our syscalls here:

// Syscall 1: printstring. Takes a char *, prints the string to the UART, returns nothing
// Syscall 2: putachar.    Takes a char, prints the character to the UART, returns nothing
// Syscall 3: getachar.    Takes no parameter, reads a character from the UART (keyboard), returns the char

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
void exception(void) {
  uint64 nr;
  uint64 param;
  uint64 retval = 0;

  // all exceptions end up here - so far, we assume that only syscalls occur
  // will have to decode the reason for the exception later!

  // TODO: copy registers a0 and a1 to variables nr and param

  // TODO: decode syscall number and jump to the appropriate function
  //       remember to check for invalid syscall numbers!

  // Here, we adjust return value - we want to return to the instruction _after_ the ecall! (at address mepc+4)
  uint64 pc = r_mepc();
  w_mepc(pc+4);

  // TODO: pass the return value back in a0

  // this function returns to ex.S
}
