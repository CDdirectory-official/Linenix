/*
 * kernel.c-graphics-setup
 * Main interactive graphics kernel. Custom setup template for open-source devs.
 */

#include "vga_io.h" // BOOM! Clean and professional hardware mapping inclusion

/* Draw a single pixel using math mapping */
void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[(y * SCREEN_WIDTH) + x] = color;
    }
}

/* Clear screen with a solid background color */
void clear_screen(unsigned char color) {
    int total_pixels = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < total_pixels; i++) {
        VGA_MEMORY[i] = color;
    }
}

/* Draw filled rectangle for UI windows or buttons */
void draw_rectangle(int x, int y, int width, int height, unsigned char color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

/* Render the initial graphical setup layout (Windows 95 style) */
void render_setup_ui() {
    clear_screen(DARK_GRAY);                    // Background
    draw_rectangle(40, 30, 240, 140, BLACK);    // Window Shadow
    draw_rectangle(42, 32, 236, 136, LIGHT_GRAY); // Window Body
    draw_rectangle(42, 32, 236, 20, BLUE);      // Title Bar
}

/* Keyboard Interrupt Handler for real-time interaction */
void keyboard_handler_c() {
    unsigned char scancode = inb(KEYBOARD_PORT);

    if (scancode & 0x80) {
        // Key released - do nothing
    } else {
        // Interactivity: Press 'Enter' (0x1C) to flash the "Next" button green
        if (scancode == 0x1C) {
            draw_rectangle(210, 140, 50, 18, GREEN);
        }
        // Interactivity: Press 'Escape' (0x01) to flash the window body red
        else if (scancode == 0x01) {
            draw_rectangle(42, 32, 236, 136, RED);
        }
    }

    // Send End of Interrupt (EOI) signal to PIC using our new outb function
    outb(PIC_COMMAND_PORT, PIC_EOI_COMMAND);
}

/* Ultimate Graphical Entry Point */
void kernel_main() {
    render_setup_ui();
    draw_rectangle(210, 140, 50, 18, WHITE); // Draw the interactive button

    while(1); // Wait for hardware interrupts
}
