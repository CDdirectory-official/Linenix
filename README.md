# kernel.c-graphics-setup 🎨🚀

A modular, bare-metal C graphics development framework running directly on raw x86 hardware. No Linux kernel, zero operating system dependencies, no bloated tracking wrappers, and completely freestanding. It structures absolute mapping parameters directly into the VGA Video Memory space (`0xA0000`) utilizing hardware **Mode 13h** ($320 \times 200$ pixels canvas, 256 colors palette).

This framework contains an ultra-lightweight decoupled system architecture including a **Virtual RAM File System (RAMFS)**, custom inline **Keyboard Hardware Interfacing Drivers**, and isolated **Subsystem Bootstrap Inits**.

## 📂 Project Structure
* `kernel/` - Machine runtime space (`boot.asm`, `kernel_entry.asm`, `kernel_graphics.c`).
* `init/` - Subsystem environment configurations & hardware verification testing (`setup_init.h`).
* `include/` - Hardware Abstraction Layers, Port registers, and color definitions (`vga_io.h`).
* `fs/` - In-memory localized file routing and configuration parsers (`ramfs.h`).
* `tools/` - Automated cross-compiling deployment tools (`Makefile`).
* `samples/` - Pre-made layout templates for graphics mapping validation.

## ⚡ Quick Start (One-Click Compilation)
Ensure you have standard environment dependencies setup (`gcc`, `ld`, `nasm`, and `qemu-system-x86_64`). Drop inside the automation command toolkit directory and trigger the deployment routines:

```bash
cd tools
make run
