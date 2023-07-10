# Build the kernel and user process binaries

CC=riscv64-unknown-elf-gcc
CFLAGS=-g -mcmodel=medany -mno-relax -I.
OBJCOPY=riscv64-unknown-elf-objcopy

KERNELDEPS = hardware.h riscv.h types.h 
KERNELOBJS = boot.o kernel.o ex.o setup.o
USERDEPS = riscv.h types.h
USEROBJS = user1.o user2.o user3.o userentry.o

%.o: %.c $(KERNELDEPS) $(USERDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.S $(KERNELDEPS) $(USERDEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:    userprogs1.h userprogs2.h userprogs3.h kernel

kernel: $(KERNELOBJS) $(KERNELDEPS) userprogs1.h userprogs2.h userprogs3.h
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany -Wl,-T kernel.ld $(KERNELOBJS) -o kernel
	$(OBJCOPY) -O binary kernel kernel.bin

userprogs1.h: $(USEROBJS) $(USERDEPS)
	$(CC) $(CFLAGS) -c user1.c
	$(CC) $(CFLAGS) -c userentry.S
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany   -Wl,-T user1.ld userentry.o user1.o -o user1
	$(OBJCOPY) -O binary user1 user1.bin
	xxd -i user1.bin > userprogs1.h

userprogs2.h: $(USEROBJS) $(USERDEPS)
	$(CC) $(CFLAGS) -c user2.c
	$(CC) $(CFLAGS) -c userentry.S
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany   -Wl,-T user2.ld userentry.o user2.o -o user2
	$(OBJCOPY) -O binary user2 user2.bin
	xxd -i user2.bin > userprogs2.h

userprogs3.h: $(USEROBJS) $(USERDEPS)
	$(CC) $(CFLAGS) -c user3.c
	$(CC) $(CFLAGS) -c userentry.S
	$(CC) -g -ffreestanding -fno-common -nostdlib -mno-relax \
	      -mcmodel=medany   -Wl,-T user3.ld userentry.o user3.o -o user3
	$(OBJCOPY) -O binary user3 user3.bin
	xxd -i user3.bin > userprogs3.h

run:	userprogs1.h userprogs2.h userprogs3.h kernel
	qemu-system-riscv64 -nographic -machine virt -smp 1 -bios none -kernel kernel -S -s
	
clean:
	-@rm -f *.o *.bin kernel user1 user2 userprogs1.h userprogs2.h userprogs3.h

