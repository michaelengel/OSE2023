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

unsigned char daszeichen = 0;
int derprozess = -1;

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

unsigned char readachar(void) {
#if 0
    while ((uart0->LSR & (1<<0)) == 0)
        ;        // do nothing - wait until bit 5 of LSR = 1
    return uart0->RBR; // then read the character
#endif
    char c = daszeichen;
    daszeichen = 0;
    return c;
}

// ask the PLIC what interrupt we should serve.
int
plic_claim(void)
{   
  int irq = *(uint32*)PLIC_MCLAIM;
  return irq;        
}   

// tell the PLIC we've served this IRQ.
void 
plic_complete(int irq) 
{       
  *(uint32*)PLIC_MCLAIM = irq;
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
uint64 ticks = 0;
      

// NPROCS is (static) maximum number of processes in the process table
pcb_struct pcb[NPROCS];

// This is the C code part of the exception handler
// "exception" is called from the assembler function "ex" in ex.S with registers saved on the stack
void exception(stackframe *s) {
  uint64 pc = 0;
  uint64 nr;
  uint64 param;
  uint64 mcause;
  uint64 retval = 0;
  int was_syscall = 0;

  nr    = s->a7;
  param = s->a0;

  pc = r_mepc();

  pcb[current_process].pc = pc;
  pcb[current_process].sp = (uint64)s;

  mcause = r_mcause();

  if ((mcause & (1ull<<63)) != 0) { // asynch
    was_syscall = 0;
    if ((mcause & ~(1ull<<63)) == 11) { // external interrupt / PLIS
      int irq = plic_claim();
      if (irq == 10) {
/*
        putachar(0x1b);
        putachar('[');
        putachar('7');
        putachar('m');
        putachar(uart0->RBR);
        putachar(0x1b);
        putachar('[');
        putachar('m');
*/
	daszeichen = uart0->RBR;
        if (derprozess >= 0) pcb[derprozess].state = READY;

        plic_complete(irq);
      }
    }
    if ((mcause & ~(1ull<<63)) == 7) { // timer interrupt / CLINT
      int interval = 1000000; // cycles; about 1/10th second in qemu.
      *(uint64*)CLINT_MTIMECMP(0) = *(uint64*)CLINT_MTIME + interval;

      ticks++;

      if ((ticks % 10) == 0) { 
               // anyone asleep?
               for (int i=0; i<NPROCS; i++) {
                 if (pcb[i].state == SLEEPING) {
                   if (ticks >= pcb[i].wakeuptime) {
                     pcb[i].state = READY;
                     pcb[i].wakeuptime = 0;
                   }
                 }
               }

               // save pc, stack
               pcb[current_process].pc = pc;
               pcb[current_process].sp = (uint64)s;
               pcb[current_process].state = READY;

               // select new process - round robin
               current_process++;
               while (pcb[current_process].state != READY) {
                 current_process++;
                 if (current_process > NPROCS) current_process = 0;
               }

               pc = pcb[current_process].pc;

               // Switch PMP memory protection to new process
               w_pmpcfg0(pcb[current_process].pmp);

               // set new process to RUNNING
               pcb[current_process].state = RUNNING;
      }
    }
  } else { 
  // not a syscall
  if(mcause != 8) {
    printstring("In exception process = ");
    printhex(current_process);
    printstring(" mcause = ");
    printhex(r_mcause());
    printstring(" mepc = ");
    printhex(r_mepc());
    printstring(" mtval = ");
    printhex(r_mtval());
    was_syscall = 0;
  }

  // syscall
  if(mcause == 8) {
    was_syscall = 1;
#if 0
    printstring(" syscall = ");
    printhex(nr);
    printstring(" param = ");
    printhex(param);
    printstring("\n");
#endif

    switch (nr) {
      case 1: printstring((char *)param);
              break;
      case 2: putachar((char)param);
              break;
      case 3: retval = readachar();
              if (retval == 0) {
                pcb[current_process].state = BLOCKED;
                derprozess = current_process;
                goto label;
              }
              break;
      case 4: pcb[current_process].wakeuptime = ticks + param * 10; // param = 1/10 sec.
              pcb[current_process].state = SLEEPING;
              pcb[current_process].pc += 4;
#if 0
              printstring("Proc  = "); printhex(current_process); 
              printstring("Param = "); printhex(param); 
              printstring("Time = "); printhex(ticks); printstring(" Wake up at "); printhex(pcb[current_process].wakeuptime);
#endif
	      goto label2;
      case 23: // printstring("yield\n");
               // save pc, stack
label:
               pcb[current_process].state = READY;
               pcb[current_process].pc = pc;
               pcb[current_process].sp = (uint64)s;
label2:

               // select new process - round robin
               current_process++;
               while (pcb[current_process].state != READY) {
                 current_process++;
                 if (current_process > NPROCS) current_process = 0;
               }
               // printstring("SCHED = "); printhex(current_process); 

               pc = pcb[current_process].pc-4;

               // Switch PMP memory protection to new process
               w_pmpcfg0(pcb[current_process].pmp);

               // set new process to RUNNING
               pcb[current_process].state = RUNNING;
               break;
      case 42: // printstring("user program returned, starting from the beginning\n");
               current_process++;
               if (current_process > 1) current_process = 0;
  
               copyprog(current_process);

               // set new process to INACTIVE
               pcb[current_process].state = INACTIVE;
              
               // for (int i=0; i<64; i+=8) { printhex(*(uint64*)(0x80100000 + i)); putachar(' '); }
               pc = (uint64)0x80100000-4;
               break;
      default: printstring("Invalid syscall: "); printhex(nr); printstring("\n");
               break;
    }
    }
  }

  // restore values for process we are going to switch to
  // adjust return value - we want to return to the instruction _after_ the ecall! (at address mepc+4)
  if (was_syscall) {
    w_mepc(pc+4);
    s->a0 = retval; // return value of syscall
  } else {
    w_mepc(pc);
  } 

  // restore old stack pointer
  s->sp = pcb[current_process].sp;

  // this function returns to ex.S
}
