/*
 * File Path: kernel/kernel_graphics.c
 * Description: Main core pipeline for Linenix Kernel. 
 * Handles Graphics rendering, Syscall routing, and Full Keyboard I/O translation.
 */

#include "../include/vga_io.h"
#include "../fs/ramfs.h"
#include "../init/setup_init.h"
#include "../drivers/video/font.h"
#include "../ipc/mailbox.h"
#include "../shell/kshell.h"
#include "../kernel/scheduler.h"

/* ===========================================================================
 * 1. GRAPHICS RENDERING SUBSYSTEM (Low-Level Pixel Mapping)
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
    draw_string("LINENIX SETUP", 50, 38, WHITE);
}

/* ===========================================================================
 * 2. CENTRAL IPC SYSCALL DISPATCHER (Software INT 0x80 Entry Point)
 * ===========================================================================
 */
void syscall_handler_c() {
    IPC_Message* msg = ipc_receive();
    if (msg != 0 && msg->command_code == 1) {
        draw_rectangle(50, 60, 220, 20, WHITE);
        draw_string("IPC:", 55, 65, BLUE);
        draw_string(msg->payload, 90, 65, BLACK);
    }
}

/* ===========================================================================
 * 3. HARDWARE INTERRUPT CALLBACK: FULL KEYBOARD I/O TRANSLATION MATRIX
 * ===========================================================================
 * DI SINI TEMPATNYA, TZE! MENGGANTIKAN DRIVER KEYBOARD YANG LAMA.
 */
void keyboard_handler_c() {
    unsigned char scancode = inb(KEYBOARD_PORT);

    // 0x80 bit mask checks if it's a Key Press (not a Key Release)
    if (!(scancode & 0x80)) { 
        
        // Handle ENTER Key -> Execute command stream inside shell
        if (scancode == 0x1C) {
            execute_command();
            
            // Trigger Software Syscall IPC via int 0x80 for logging metrics
            ipc_send(9, 1, "EXEC_CMD");
            __asm__ volatile("int $0x80");
        }
        // Handle BACKSPACE Key -> Destructive character deletion
        else if (scancode == 0x0E) {
            if (buffer_index > 0) {
                buffer_index--;
                shell_buffer[buffer_index] = '\0';
                // Wipe the character from GUI window by drawing a small gray box over it
                draw_rectangle(45 + ((buffer_index + 1) * 8), SHELL_Y, 8, 12, LIGHT_GRAY);
            }
        }
        // Handle SPACEBAR Key
        else if (scancode == 0x39) {
            shell_input_char(' ');
        }
        /* COMPLETE IBM PC/AT SCANCODE-TO-ASCII LOOKUP TABLE MATRIX */
        else {
            char ascii_char = 0;
            switch (scancode) {
                case 0x1E: ascii_char = 'a'; break; case 0x30: ascii_char = 'b'; break;
                case 0x2E: ascii_char = 'c'; break; case 0x20: ascii_char = 'd'; break;
                case 0x12: ascii_char = 'e'; break; case 0x21: ascii_char = 'f'; break;
                case 0x22: ascii_char = 'g'; break; case 0x23: ascii_char = 'h'; break;
                case 0x17: ascii_char = 'i'; break; case 0x24: ascii_char = 'j'; break;
                case 0x25: ascii_char = 'k'; break; case 0x26: ascii_char = 'l'; break;
                case 0x32: ascii_char = 'm'; break; case 0x31: ascii_char = 'n'; break;
                case 0x18: ascii_char = 'o'; break; case 0x19: ascii_char = 'p'; break;
                case 0x10: ascii_char = 'q'; break; case 0x13: ascii_char = 'r'; break;
                case 0x1F: ascii_char = 's'; break; case 0x14: ascii_char = 't'; break;
                case 0x16: ascii_char = 'u'; break; case 0x2F: ascii_char = 'v'; break;
                case 0x11: ascii_char = 'w'; break; case 0x2D: ascii_char = 'x'; break;
                case 0x15: ascii_char = 'y'; break; case 0x2C: ascii_char = 'z'; break;
                
                // Number row keys allocation params
                case 0x02: ascii_char = '1'; break; case 0x03: ascii_char = '2'; break;
                case 0x04: ascii_char = '3'; break; case 0x05: ascii_char = '4'; break;
                case 0x06: ascii_char = '5'; break; case 0x07: ascii_char = '6'; break;
                case 0x08: ascii_char = '7'; break; case 0x09: ascii_char = '8'; break;
                case 0x0A: ascii_char = '9'; break; case 0x0B: ascii_char = '0'; break;
                default: break; // Ignore special unmapped keys for now
            }

            // Pipe valid translation characters straight to the shell engine
            if (ascii_char != 0) {
                shell_input_char(ascii_char);
            }
        }
    }
    
    // Notify the PIC that hardware interrupt servicing is completed
    outb(PIC_COMMAND_PORT, PIC_EOI_COMMAND);
}

/* ===========================================================================
 * 4. KERNEL ENTRY POINT SUBROUTINE (Called by kernel_entry.asm)
 * ===========================================================================
 */
void kernel_main() {
    bootstrap_system();
    draw_string("NEXT", 222, 145, BLACK);
    
    // Initialize the console interface terminal layout
    init_shell();
    
    while(1); // Stay alive and let PIT timer interrupts swap task contexts
}
