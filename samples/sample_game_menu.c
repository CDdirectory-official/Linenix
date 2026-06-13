/* * Sample: Retro Game Menu Border Layout
 * Path: samples/sample_game_menu.c
 */

#include "../vga_io.h"

void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[(y * SCREEN_WIDTH) + x] = color;
    }
}

void draw_rectangle(int x, int y, int width, int height, unsigned char color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

void kernel_main() {
    // 1. Clear screen to pitch black
    draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);

    // 2. Draw a big green cyber-style border container for a game menu
    draw_rectangle(10, 10, 300, 180, GREEN);      // Outer green box
    draw_rectangle(14, 14, 292, 172, BLACK);      // Hollow out the inside

    // 3. Draw a small selection box inside (Simulating a menu item)
    draw_rectangle(30, 30, 100, 20, CYAN);

    while(1); // Halt