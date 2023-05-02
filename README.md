# OSE2023
Code for the course operating systems engineering at Uni Bamberg (Summer 2023)

You need the following tools on a Unix-like system (Linux, macOS, Windows with WSL):

- RISC-V 64 bit cross compilation toolchain (gcc or clang)
  - probably available as package in your Linux package manager (also on WSL)
  - [homebrew package for macOS](https://github.com/riscv-software-src/homebrew-riscv)

- The [qemu emulator](https://www.qemu.org)
  - again, probably available as package in your Linux package manager (also on WSL)
  - [qemu for macOS](https://wiki.qemu.org/Hosts/Mac)
  
Compile using the provided shellscript `build.sh`

Run the program using `qemu-system-riscv64 -nographic -machine virt -smp 1 -bios none -kernel ./hello`

You can attach a debugger by running qemu as follows:
`qemu-system-riscv64 -nographic -machine virt -smp 1 -bios none -kernel ./hello -S -s`

In another terminal, you can then start the debugger:
`riscv64-unknown-elf-gdb hello` 
and then at the (gdb) prompt enter
`target remote localhost:1234`
to connect the debugger to the qemu emulator.

Useful links:

- [Daniel Mangum's RISC-V debugging tutorial](https://danielmangum.com/posts/risc-v-bytes-qemu-gdb/)

