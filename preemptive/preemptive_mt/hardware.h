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

// core local interruptor (CLINT), which contains the timer.
#define CLINT 0x2000000L
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8*(hartid))
#define CLINT_MTIME (CLINT + 0xBFF8) // cycles since boot.

// platform level interrupt controller (PLIC)
#define PLIC 0x0c000000L
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)
#define PLIC_MENABLE (PLIC + 0x2000)
#define PLIC_SENABLE (PLIC + 0x2080)
#define PLIC_MPRIORITY (PLIC + 0x200000)
#define PLIC_SPRIORITY (PLIC + 0x201000)
#define PLIC_MCLAIM (PLIC + 0x200004)
#define PLIC_SCLAIM (PLIC + 0x201004)

#define UART0_IRQ 10

