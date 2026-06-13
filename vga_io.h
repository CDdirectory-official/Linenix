/*
 * vga_io.h - Hardware abstraction layer for TzeOS Setup
 * Contains VGA graphics definitions and raw port I/O helpers.
 */

#ifndef VGA_IO_H
#define VGA_IO_H

// Screen dimensions for VGA Mode 13h
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VGA_MEMORY (unsigned char*)0xA0000

// Hardware Ports
#define KEYBOARD_PORT 0x60
#define PIC_EOI_COMMAND 0x20
#define PIC_COMMAND_PORT 0x20

// Standard 256 VGA Color Codes (First 16 standard)
#define BLACK        0
#define BLUE         1
#define GREEN        2
#define CYAN         3
#define RED          4
#define MAGENTA      5
#define LIGHT_GRAY   7
#define DARK_GRAY    8
#define WHITE        15

/* Read raw byte from hardware port (Inline Assembly) */
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

/* Send raw byte to hardware port (Inline Assembly) */
static inline void outb(unsigned short port, unsigned char data) {
    __asm__ volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

#endif
