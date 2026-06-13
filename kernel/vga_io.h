/*
 * File Path: include/vga_io.h
 * Description: Hardware Abstraction Layer for VGA Mode 13h and port I/O.
 */

#ifndef VGA_IO_H
#define VGA_IO_H

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VGA_MEMORY (unsigned char*)0xA0000

/* Hardware Port Map */
#define KEYBOARD_PORT 0x60
#define PIC_EOI_COMMAND 0x20
#define PIC_COMMAND_PORT 0x20

/* Standard VGA 256-Color Palette (First 16 Colors) */
#define BLACK        0
#define BLUE         1
#define GREEN        2
#define CYAN         3
#define RED          4
#define MAGENTA      5
#define LIGHT_GRAY   7
#define DARK_GRAY    8
#define WHITE        15

/**
 * Read a raw byte from an assembly hardware port.
 */
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

/**
 * Write a raw byte out to an assembly hardware port.
 */
static inline void outb(unsigned short port, unsigned char data) {
    __asm__ volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

#endif
