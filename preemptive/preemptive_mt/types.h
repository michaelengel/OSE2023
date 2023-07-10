typedef unsigned char uint8_t;
typedef unsigned int uint32;
typedef unsigned long long int uint64;

// (static) maximum number of processes
#define NPROCS 16

enum pstate { INACTIVE=0, READY, RUNNING, BLOCKED, SLEEPING };

typedef struct {
  uint64 pc;  // program counter
  uint64 sp;  // stack pointer
  enum pstate state;   // process state
  uint64 pmp; // pmp config
  uint64 wakeuptime; 
} pcb_struct;

typedef struct {
        uint64 ra;
        uint64 sp;
        uint64 gp;
        uint64 tp;
        uint64 t0;
        uint64 t1;
        uint64 t2;
        uint64 s0;
        uint64 s1;
        uint64 a0;
        uint64 a1;
        uint64 a2;
        uint64 a3;
        uint64 a4;
        uint64 a5;
        uint64 a6;
        uint64 a7;
        uint64 s2;
        uint64 s3;
        uint64 s4;
        uint64 s5;
        uint64 s6;
        uint64 s7;
        uint64 s8;
        uint64 s9;
        uint64 s10;
        uint64 s11;
        uint64 t3;
        uint64 t4;
        uint64 t5;
        uint64 t6;
} stackframe;
