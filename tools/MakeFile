# ==============================================================================
# tools/Makefile - Automated Bare-Metal Build System
# Multi-stage compilation for the graphics setup kernel framework.
# ==============================================================================

# Compiler and Linker flags for pure freestanding x86 execution
CC = gcc
CFLAGS = -m32 -c -ffreestanding -O2 -Wall -Wextra
ASM = nasm
ASMFLAGS = -f elf32
LD = ld
LDFLAGS = -m elf_i386 -Ttext 0x1000 --oformat binary

# Output binaries
TARGET = ../tzeos_setup.bin
ENTRY_OBJ = k_entry.o
KERNEL_OBJ = kernel.o

.PHONY: all clean run

# Default target: Build everything
all: $(TARGET)

# Stage 1: Assemble the 32-bit boot entry point
$(ENTRY_OBJ): ../kernel_entry.asm
	@echo "[ASM] Assembling bootloader entry..."
	@$(ASM) $(ASMFLAGS) ../kernel_entry.asm -o $(ENTRY_OBJ)

# Stage 2: Compile the interactive graphics core kernel
$(KERNEL_OBJ): ../kernel_graphics.c ../vga_io.h ../fs/ramfs.h
	@echo "[CC] Compiling core graphics kernel..."
	@$(CC) $(CFLAGS) ../kernel_graphics.c -o $(KERNEL_OBJ)

# Stage 3: Link objects into a raw, flat binary mapped at 0x1000
$(TARGET): $(ENTRY_OBJ) $(KERNEL_OBJ)
	@echo "[LD] Linking objects into raw machine binary..."
	@$(LD) $(LDFLAGS) $(ENTRY_OBJ) $(KERNEL_OBJ) -o $(TARGET)
	@echo "[SUCCESS] Build complete! Output located at: $(TARGET)"

# Shortcut: Clean up compilation trash files
clean:
	@echo "[CLEAN] Removing object files..."
	@rm -f *.o $(TARGET)

# Shortcut: Compile and instantly launch the virtual machine
run: all
	@echo "[QEMU] Firing up the kernel framework..."
	@qemu-system-x86_64 -drive format=raw,file=$(TARGET)