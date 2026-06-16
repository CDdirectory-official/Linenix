/*
 * File Path: shell/kshell.h
 * Description: Interactive Kernel Command Line Interface (Shell).
 */

#ifndef KSHELL_H
#define KSHELL_H

#include "../include/vga_io.h"
#include "../drivers/video/font.h"
#include "../fs/ramfs.h"

#define BUFFER_MAX 32
#define SHELL_Y 120  // Row inside the window canvas where the shell starts

static char shell_buffer[BUFFER_MAX];
static int buffer_index = 0;

// External rendering routines from kernel_graphics.c
extern void draw_rectangle(int x, int y, int width, int height, unsigned char color);
extern RAM_File virtual_disk[];
extern int file_count;

/**
 * Clears the shell terminal line display.
 */
void clear_shell_line() {
    draw_rectangle(45, SHELL_Y, 230, 12, LIGHT_GRAY);
    draw_string(">", 45, SHELL_Y, BLACK);
}

/**
 * Initializes the shell interface.
 */
void init_shell() {
    buffer_index = 0;
    shell_buffer[0] = '\0';
    draw_rectangle(44, SHELL_Y - 4, 232, 18, BLACK); // Shell Border Box
    clear_shell_line();
}

/**
 * Executes typed commands when Enter is pressed.
 */
void execute_command() {
    // Redraw the command execution log space
    draw_rectangle(45, SHELL_Y - 25, 230, 20, LIGHT_GRAY);

    int match = 0;

    // Command 1: HELP
    if (shell_buffer[0] == 'h' && shell_buffer[1] == 'e' && shell_buffer[2] == 'l' && shell_buffer[3] == 'p' && shell_buffer[4] == '\0') {
        draw_string("COMMANDS: HELP, CLEAR, LS", 45, SHELL_Y - 20, BLUE);
        match = 1;
    }
    // Command 2: CLEAR
    else if (shell_buffer[0] == 'c' && shell_buffer[1] == 'l' && shell_buffer[2] == 'e' && shell_buffer[3] == 'a' && shell_buffer[4] == 'r' && shell_buffer[5] == '\0') {
        draw_rectangle(45, SHELL_Y - 60, 230, 55, LIGHT_GRAY); // Clean the upper log space
        match = 1;
    }
    // Command 3: LS (List files inside RAMFS!)
    else if (shell_buffer[0] == 'l' && shell_buffer[1] == 's' && shell_buffer[2] == '\0') {
        if (file_count == 0) {
            draw_string("RAMFS IS EMPTY", 45, SHELL_Y - 20, RED);
        } else {
            // Print out names of all injected files dynamically
            int x_offset = 45;
            for (int i = 0; i < file_count; i++) {
                draw_string(virtual_disk[i].name, x_offset, SHELL_Y - 20, MAGENTA);
                x_offset += (8 * 10); // Advance font space horizontally
            }
        }
        match = 1;
    }

    if (!match && buffer_index > 0) {
        draw_string("UNKNOWN COMMAND, BOZO!", 45, SHELL_Y - 20, RED);
    }

    // Reset buffer for the next input cycle
    buffer_index = 0;
    shell_buffer[0] = '\0';
    clear_shell_line();
}

/**
 * Appends safe alphanumeric keystrokes into the active command array stream.
 */
void shell_input_char(char c) {
    if (buffer_index < BUFFER_MAX - 1) {
        shell_buffer[buffer_index] = c;
        shell_buffer[buffer_index + 1] = '\0';
        buffer_index++;
        
        // Print character directly on the shell line canvas
        char str[2] = {c, '\0'};
        draw_string(str, 45 + (buffer_index * 8), SHELL_Y, BLACK);
    }
}

#endif