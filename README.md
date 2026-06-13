**# kernel.c-graphics-setup 

A bare-metal C graphics engine template running directly on the hardware. No OS, no Linux kernel, no standard libraries, and zero bloat. It directly maps pixels into the x86 VGA Video Memory (`0xA0000`) in **Mode 13h** (320x200 resolution, 256 colors).

This is a dynamic boilerplate for people who want to build custom graphical setups, retro game engines, or Windows 95-style GUIs from scratch without configuration headaches.

##  Included Primitive Functions
* `put_pixel(x, y, color)` - Direct mathematical pixel plotting.
* `clear_screen(color)` - Fast full-buffer screen clearing.
* `draw_rectangle(x, y, width, height, color)` - Nested-loop drawing for UI windows, buttons, or taskbars.

##  Quick Start (Compile & Boot)
> **Note:** Ensure your bootloader puts the CPU into VGA Mode 13h before jumping to this kernel.

Run these commands in your terminal to compile and run the GUI template inside QEMU:

```bash
# 1. Assemble the 32-bit boot entry
nasm -f elf32 kernel_entry.asm -o k_entry.o

# 2. Compile the graphics kernel without standard libraries
gcc -m32 -c kernel_graphics.c -o kernel.o -ffreestanding

# 3. Link them into a raw binary mapped at 0x1000
ld -m elf_i386 -o kernel.bin -Ttext 0x1000 k_entry.o kernel.o --oformat binary

# 4. Fire up the Virtual Machine and watch the pixel magic!
qemu-system-x86_64 -drive format=raw,file=kernel.bin**
