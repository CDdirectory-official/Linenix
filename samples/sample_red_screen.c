/* * Sample: Full Red Screen Initialization
 * Path: samples/sample_red_screen.c
 */

#include "../vga_io.h"

void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[(y * SCREEN_WIDTH) + x] = color;
    }
}

void kernel_main() {
    // Fill the entire 320x200 screen with RED (Color code: 4)
    int total_pixels = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < total_pixels; i++) {
        VGA_MEMORY[i] = RED;
    }

    while(1); // Halt
}