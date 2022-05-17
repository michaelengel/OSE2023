typedef char uint8_t;

__attribute__ ((aligned (16))) char stack0[4096];

struct uart {
   union {
     uint8_t THR; // W = transmit hold register (offset 0)
     uint8_t RBR; // R = receive buffer register (also offset 0)
     uint8_t DLL; // R/W = divisor latch low (offset 0 when DLAB=1)
   };
   union {
     uint8_t IER; // R/W = interrupt enable register (offset 1)
     uint8_t DLH; // R/W = divisor latch high (offset 1 when DLAB=1)
   };
   union {
     uint8_t IIR; // R = interrupt identif. reg. (offset 2)
     uint8_t FCR; // W = FIFO control reg. (also offset 2)
   };
   uint8_t LCR; // R/W = line control register (offset 3)
   uint8_t MCR; // R/W = modem control register (offset 4)
   uint8_t LSR; // R   = line status register (offset 5)
};

/*
struct uart {
   uint8_t THR; // transmit hold register
   uint8_t IER;
   uint8_t IIR;
   uint8_t LCR;
   uint8_t MCR;
   uint8_t LSR; // line status register
};
*/

volatile struct uart* uart0 = (volatile struct uart *)0x10000000;

void putachar(char c) {
    while ((uart0->LSR & (1<<5)) == 0)
        ;        // do nothing - wait until bit 5 of LSR = 1
    uart0->THR = c; // then write the character
}

void printstring(char *s) {
    while (*s) {     // as long as the character is not null
        putachar(*s); // output the character
        s++;         // and progress to the next character
    }
}

int main(void) {
    printstring("Hallo Bambärch!\n");
    return 0;
}


