/*
 * File Path: kernel/kernel_graphics.c
 * Description: Fully interactive core loop handling keyboard-to-shell stream piping.
 */

#include "../include/vga_io.h"
#include "../fs/ramfs.h"
#include "../init/setup_init.h"
#include "../drivers/video/font.h"
#include "../ipc/mailbox.h"
#include "../shell/kshell.h" // BOOM! Interactive Kernel Shell Mounted

/* ===========================================================================
 * GRAPHICS RENDERING ARCHITECTURE (Unchanged core systems)
 * ===========================================================================
 */
void put_pixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[(y * SCREEN_WIDTH) + x] = color;
    }
}
void clear_screen(unsigned char color) {
    int total_pixels = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < total_pixels; i++) { VGA_MEMORY[i] = color; }
}
void draw_rectangle(int x, int y, int width, int height, unsigned char color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) { put_pixel(x + j, y + i, color); }
    }
}
void render_setup_ui() {
    clear_screen(DARK_GRAY);
    draw_rectangle(40, 30, 240, 140, BLACK);
    draw_rectangle(42, 32, 236, 136, LIGHT_GRAY);
    draw_rectangle(42, 32, 236, 20, BLUE);
    draw_string("TzeOS SETUP", 50, 38, WHITE);
}

void syscall_handler_c() {
    IPC_Message* msg = ipc_receive();
    if (msg != 0 && msg->command_code == 1) {
        draw_rectangle(50, 60, 220, 20, WHITE);
        draw_string("IPC:", 55, 65, BLUE);
        draw_string(msg->payload, 90, 65, BLACK);
    }
}

/* ===========================================================================
 * HARDWARE INTERRUPT CALLBACK ROUTINE (RE-ENGINEERED FOR ASCII STREAM)
 * ===========================================================================
 */
void keyboard_handler_c() {
    unsigned char scancode = inb(KEYBOARD_PORT);

    if (!(scancode & 0x80)) { // Keypress event detected
        
        // Handle ENTER key (Execute input)
        if (scancode == 0x1C) {
            execute_command();
            
            // Retain original IPC payload simulation trigger
            draw_rectangle(210, 140, 50, 18, GREEN);
            draw_string("NEXT", 222, 145, WHITE);
            ipc_send(9, 1, "EXEC_CMD");
            __asm__ volatile("int $0x80");
        }
        // Handle ESCAPE key (Trigger panic fallback mode)
        else if (scancode == 0x01) {
            draw_rectangle(42, 52, 236, 116, RED);
            draw_string("PANIC", 130, 95, WHITE);
        }
        /* ASCII SCANCODE TRANSLATION TABLE MATRIX */
        else if (scancode == 0x23) { shell_input_char('h'); }
        else if (scancode == 0x12) { shell_input_char('e'); }
        else if (scancode == 0x26) { shell_input_char('l'); }
        else if (scancode == 0x19) { shell_input_char('p'); }
        else if (scancode == 0x2E) { shell_input_char('c'); }
        else if (scancode == 0x1F) { shell_input_char('s'); }
        else if (scancode == 0x1E) { shell_input_char('a'); }
        else if (scancode == 0x13) { shell_input_char('r'); }
    }
    outb(PIC_COMMAND_PORT, PIC_EOI_COMMAND);
}

void kernel_main() {
    bootstrap_system();
    draw_string("NEXT", 222, 145, BLACK);
    
    // Boot the interactive console console
    init_shell();
    
    while(1);
}
