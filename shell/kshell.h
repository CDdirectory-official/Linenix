/*
 * File Path: shell/kshell.h
 * Description: Command Interpreter Framework and Auto-Simulation Suite for Linenix.
 */

#ifndef KSHELL_H
#define KSHELL_H

#define SHELL_Y 120
extern void draw_string(const char* str, int x, int y, unsigned char color);
extern void draw_rectangle(int x, int y, int width, int height, unsigned char color);
extern void linenix_panic(const char* error_code, const char* description);
extern char* read_file(const char* filename);

char shell_buffer[64];
int buffer_index = 0;

void init_shell() {
    draw_rectangle(42, 100, 236, 1, BLACK); // Divider line
    draw_string("Linenix Interactive Console Active", 45, 105, BLUE);
    draw_string("> ", 45, SHELL_Y, BLACK);
}

void shell_input_char(char c) {
    if (buffer_index < 63) {
        shell_buffer[buffer_index] = c;
        char str[2] = {c, '\0'};
        draw_string(str, 45 + (buffer_index * 8) + 16, SHELL_Y, BLACK);
        buffer_index++;
        shell_buffer[buffer_index] = '\0';
    }
}

// Custom simple string comparison since we are in bare-metal
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void execute_command();

/* ===========================================================================
 * AUTOMATED TESTING MATRIX (For Future Users and CI/CD Pipelines)
 * ===========================================================================
 */
void run_automated_testing_suite() {
    draw_string("[SIM] Initiating Core Subsystem Tests...", 45, 50, BLUE);
    
    // Test 1: VFS Integrity Check
    char* file_check = read_file("setup.inf");
    if (file_check != 0) {
        draw_string("TEST 1: VFS RAMFS MAP -> PASSED", 45, 62, 2); // Hijau
    } else {
        draw_string("TEST 1: VFS RAMFS MAP -> FAILED", 45, 62, 4); // Merah
    }

    // Test 2: Auto-Type Script Simulation
    draw_string("TEST 2: SIMULATING EXPLOIT INPUT...", 45, 74, BLACK);
    
    // Injecting fake "sudo" command into the buffer to test security panic
    shell_buffer[0] = 's'; shell_buffer[1] = 'u'; shell_buffer[2] = 'd'; shell_buffer[3] = 'o';
    shell_buffer[4] = '\0';
    
    // Tiny delay emulator loop before disaster
    for(volatile int i = 0; i < 9000000; i++);

    // TRIGGER DOOMSDAY INTERPRETER
    execute_command();
}

void execute_command() {
    if (strcmp(shell_buffer, "exec_cmd_bozo_999") == 0) {
        draw_rectangle(45, SHELL_Y + 15, 230, 20, LIGHT_GRAY);
        draw_string("ARG CODE VALIDATED. ACCESS GRANTED.", 45, SHELL_Y + 15, 2);
    }
    else if (strcmp(shell_buffer, "sudo") == 0 || strcmp(shell_buffer, "rm -rf") == 0) {
        linenix_panic("0xDEADBEEF", "SECURITY EXPLOIT TRAPPED VIA AUTO-TEST.");
    }
    else if (strcmp(shell_buffer, "help") == 0) {
        draw_string("COMMANDS: help, ls, clear, sim", 45, SHELL_Y + 15, BLACK);
    }
    else if (strcmp(shell_buffer, "sim") == 0) {
        run_automated_testing_suite();
    }
    else {
        draw_string("UNKNOWN COMMAND, BOZO!", 45, SHELL_Y + 15, 4);
    }

    // Wipe buffer arrays
    buffer_index = 0;
    for(int i = 0; i < 64; i++) shell_buffer[i] = '\0';
}

#endif
