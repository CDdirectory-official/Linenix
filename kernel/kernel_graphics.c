/*
 * kernel.c-graphics-setup
 * File Path: kernel/kernel_graphics.c
 * ---------------------------------------------------------------------------
 * The Ultimate Bare-Metal Interactive Graphics Kernel & Setup Template.
 * Fully decoupled architecture featuring VGA Mode 13h, Input Drivers, and RAMFS.
 * * If you are too lazy to customize this, go to Torvalds' repo, bozo!
 * ---------------------------------------------------------------------------
 */

#include "../include/vga_io.h"  // Path updated: exits kernel/, enters include/
#include "../fs/ramfs.h"        // Path updated: exits kernel/, enters fs/
#include "../init/setup_init.h" // Includes the subsystem initialization routines

/* ===========================================================================
 * PURE HARDWARE GRAPHICS ENGINE (VGA MODE 13h)
 * ===========================================================================
 */

/**
 * Plot a single pixel directly into the VGA video memory buffer.
 * Memory Formula: Video_Address + (Y * 320) + X
 */
void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[(y * SCREEN_WIDTH) + x] = color;
    }
}

/**
 * Flood-fill the entire screen with a specific VGA color palette byte.
 */
void clear_screen(unsigned char color) {
    int total_pixels = SCREEN_WIDTH * SCREEN_HEIGHT; // 64,000 pixels total
    for (int i = 0; i < total_pixels; i++) {
        VGA_MEMORY[i] = color;
    }
}

/**
 * Draw a solid filled rectangle. 
 * Perfect for rendering low-level GUI windows, buttons, or dialog boxes.
 */
void draw_rectangle(int x, int y, int width, int height, unsigned char color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

/**
 * Render the baseline graphical environment layout (Retro Windows 95 Style).
 */
void render_setup_ui() {
    clear_screen(DARK_GRAY);                      // OS Desktop Background
    draw_rectangle(40, 30, 240, 140, BLACK);      // Window Drop-Shadow
    draw_rectangle(42, 32, 236, 136, LIGHT_GRAY); // Main Window Canvas Body
    draw_rectangle(42, 32, 236, 20, BLUE);        // Graphical Title Bar
}

/* ===========================================================================
 * BARE-METAL KEYBOARD INTERRUPT DRIVER (PORT I/O)
 * ===========================================================================
 */

/**
 * Core Keyboard Interrupt Handler.
 * Reads scancodes directly from Port 0x60 and manipulates the UI in real-time.
 */
void keyboard_handler_c() {
    // Read raw hardware data from keyboard controller register
    unsigned char scancode = inb(KEYBOARD_PORT);

    // Bit 0x80 checks if the key was released. We only care about keypresses.
    if (scancode & 0x80) {
        // Key released - ignored
    } else {
        /* INTERACTIVE UI ACTIONS */
        
        // Action 1: If user presses 'Enter' (Scancode 0x1C), flash the "Next" button green
        if (scancode == 0x1C) {
            draw_rectangle(210, 140, 50, 18, GREEN);
        }
        
        // Action 2: If user presses 'Escape' (Scancode 0x01), flash window body red (Error simulation)
        else if (scancode == 0x01) {
            draw_rectangle(42, 52, 236, 116, RED);
        }
    }

    // MANDATORY: Send End of Interrupt (EOI) signal back to the PIC hardware chip.
    // Without this line, the keyboard will lock up after the first keypress.
    outb(PIC_COMMAND_PORT, PIC_EOI_COMMAND);
}

/* ===========================================================================
 * KERNEL ENTRY POINT
 * ===========================================================================
 */
void kernel_main() {
    // Delegate all dirty work to the initialization subsystem
    bootstrap_system();

    // System initialized. CPU safely enters infinite loop waiting for hardware interrupts.
    while(1);
}
