/*
 * Kernel Graphics Engine & Input Driver for Setup
 * Mode 13h (320x200, 256 Colors) + Bare-Metal Keyboard Interfacing.
 * Target Memory: 0xA0000 | Keyboard Port: 0x60
 */

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define VGA_MEMORY (unsigned char*)0xA0000

/* Color Palette Constants */
#define BLACK        0
#define BLUE         1
#define GREEN        2
#define CYAN         3
#define RED          4
#define MAGENTA      5
#define WHITE        15
#define DARK_GRAY    8
#define LIGHT_GRAY   7

/* Read raw byte from hardware port */
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

/* Draw a single pixel */
void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[(y * SCREEN_WIDTH) + x] = color;
    }
}

/* Clear screen with solid color */
void clear_screen(unsigned char color) {
    int total_pixels = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < total_pixels; i++) {
        VGA_MEMORY[i] = color;
    }
}

/* Draw filled rectangle (For UI elements) */
void draw_rectangle(int x, int y, int width, int height, unsigned char color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

/* * Render the static GUI Setup Environment 
 * Windows 95 Style layout boilerplate.
 */
void render_setup_ui() {
    clear_screen(DARK_GRAY);                    // Background
    draw_rectangle(40, 30, 240, 140, BLACK);    // Window Shadow
    draw_rectangle(42, 32, 236, 136, LIGHT_GRAY); // Window Body
    draw_rectangle(42, 32, 236, 20, BLUE);      // Title Bar
}

/* * Keyboard Interrupt Handler
 * This reacts directly to user inputs and updates the graphics layout dynamically!
 */
void keyboard_handler_c() {
    unsigned char scancode = inb(0x60);

    if (scancode & 0x80) {
        // Key released - do nothing
    } else {
        // Example: If user presses 'Enter' (Scancode 0x1C), simulate clicking "Next"
        if (scancode == 0x1C) {
            // Flash the button green to show interaction!
            draw_rectangle(210, 140, 50, 18, GREEN);
        }
        // Example: If user presses 'Escape' (Scancode 0x01), cancel and flash red
        else if (scancode == 0x01) {
            draw_rectangle(42, 32, 236, 136, RED); // Screen flashes red on cancel
        }
    }

    // Send EOI to PIC to accept the next keystroke
    __asm__("mov $0x20, %al; out %al, $0x20");
}

/* Ultimate Graphical Entry Point */
void kernel_main() {
    // 1. Draw the initial setup screen layout
    render_setup_ui();

    // 2. Draw the "Next" button in generic white first
    draw_rectangle(210, 140, 50, 18, WHITE);

    // 3. Enter infinite loop, waiting for human keyboard inputs to change the graphics
    while(1);
}
