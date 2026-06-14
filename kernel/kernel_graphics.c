/*
 * File Path: kernel/kernel_graphics.c
 * Description: Main bare-metal runtime loop execution and system orchestration.
 */

#include "../include/vga_io.h"
#include "../fs/ramfs.h"
#include "../init/setup_init.h"

/* ===========================================================================
 * GRAPHICS RENDERING ARCHITECTURE
 * ===========================================================================
 */

void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[(y * SCREEN_WIDTH) + x] = color;
    }
}

void clear_screen(unsigned char color) {
    int total_pixels = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < total_pixels; i++) {
        VGA_MEMORY[i] = color;
    }
}

void draw_rectangle(int x, int y, int width, int height, unsigned char color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

void render_setup_ui() {
    clear_screen(DARK_GRAY);
    draw_rectangle(40, 30, 240, 140, BLACK);
    draw_rectangle(42, 32, 236, 136, LIGHT_GRAY);
    draw_rectangle(42, 32, 236, 20, BLUE);
}

/* ===========================================================================
 * SUBSYSTEM INTERRUPT CALLBACK
 * ===========================================================================
 */

void keyboard_handler_c() {
    unsigned char scancode = inb(KEYBOARD_PORT);

    if (!(scancode & 0x80)) { // Active Keypress events
        if (scancode == 0x1C) {       // Enter Key -> Flash button GREEN
            draw_rectangle(210, 140, 50, 18, GREEN);
        } else if (scancode == 0x01) { // Escape Key -> Crash/Flash window RED
            draw_rectangle(42, 52, 236, 116, RED);
        }
    }
    outb(PIC_COMMAND_PORT, PIC_EOI_COMMAND);
}

/* ===========================================================================
 * MASTER RUNTIME TARGET ENTRY
 * ===========================================================================
 */
void kernel_main() {
    bootstrap_system();
    while(1);
}
