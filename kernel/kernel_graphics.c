/*
 * File Path: kernel/kernel_graphics.c
 * Description: Core kernel dispatcher linked with Software Interrupt IPC layers.
 */

#include "../include/vga_io.h"
#include "../fs/ramfs.h"
#include "../init/setup_init.h"
#include "../drivers/video/font.h"
#include "../ipc/mailbox.h" // BOOM! IPC Mailbox Subsystem Mounted

/* ===========================================================================
 * GRAPHICS RENDERING ARCHITECTURE (Existing codes)
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

/* ===========================================================================
 * CENTRAL IPC SYSCALL DISPATCHER (INT 0x80 ENTRY POINT)
 * ===========================================================================
 */
void syscall_handler_c() {
    // Process messages stuck inside the IPC Mailbox Queue
    IPC_Message* msg = ipc_receive();
    if (msg != 0) {
        // Command Code 1: Action Trigger for UI Updates via IPC
        if (msg->command_code == 1) {
            draw_rectangle(50, 60, 220, 40, WHITE);
            draw_string("IPC MSG RECEIVED:", 55, 65, BLUE);
            draw_string(msg->payload, 55, 80, BLACK); // Print the IPC message payload on GUI!
        }
    }
}

/* ===========================================================================
 * HARDWARE INTERRUPT CALLBACK
 * ===========================================================================
 */
void keyboard_handler_c() {
    unsigned char scancode = inb(KEYBOARD_PORT);

    if (!(scancode & 0x80)) {
        if (scancode == 0x1C) { // Enter Key -> Trigger Software IPC Payload Simulation!
            draw_rectangle(210, 140, 50, 18, GREEN);
            draw_string("NEXT", 222, 145, WHITE);

            // Simulasikan pengiriman pesan IPC dari Subsistem Luar (ID: 9) ke Kernel
            ipc_send(9, 1, "SYS_INIT_OK");

            // PICU SOFTWARE INTERRUPT INT 0x80! 
            // Ini akan memaksa CPU melompat ke syscall_handler_c lewat IDT
            __asm__ volatile("int $0x80");

        } else if (scancode == 0x01) { // Escape Key
            draw_rectangle(42, 52, 236, 116, RED);
            draw_string("PANIC", 130, 95, WHITE);
        }
    }
    outb(PIC_COMMAND_PORT, PIC_EOI_COMMAND);
}

void kernel_main() {
    bootstrap_system();
    draw_string("NEXT", 222, 145, BLACK);
    while(1);
}
