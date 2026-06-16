/*
 * File Path: shell/kshell.h
 * Description: Advanced Command Interpreter Framework, Auto-Simulation Suite,
 * and Serial Network Logging Hook for Linenix Kernel.
 */

#ifndef KSHELL_H
#define KSHELL_H

#define SHELL_Y 120

/* ===========================================================================
 * EXTERNAL RESOURCE LINKING (Connecting to Core Kernel & Graphics)
 * ===========================================================================
 */
extern void draw_string(const char* str, int x, int y, unsigned char color);
extern void draw_rectangle(int x, int y, int width, int height, unsigned char color);
extern void linenix_panic(const char* error_code, const char* description);
extern char* read_file(const char* filename);
extern void write_serial_string(const char* str); // Serial interface connection hook

/* Global variables for shell data streams */
char shell_buffer[64];
int buffer_index = 0;

/**
 * Initializes the console user interface layout inside the VGA graphics window.
 */
void init_shell() {
    draw_rectangle(42, 100, 236, 1, BLACK); // Divider border line
    draw_string("Linenix Interactive Console Active", 45, 105, BLUE);
    draw_string("> ", 45, SHELL_Y, BLACK);
}

/**
 * Pipes real-time keyboard ASCII inputs directly onto the GUI canvas window
 * and pushes them into the kernel memory string stream buffer.
 */
void shell_input_char(char c) {
    if (buffer_index < 63) {
        shell_buffer[buffer_index] = c;
        char str[2] = {c, '\0'};
        // Render character real-time with an 8-pixel spacing font offset
        draw_string(str, 45 + (buffer_index * 8) + 16, SHELL_Y, BLACK);
        buffer_index++;
        shell_buffer[buffer_index] = '\0';
    }
}

/**
 * Bare-metal string comparison engine blueprint (Standard library strcmp fallback)
 */
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { 
        s1++; 
        s2++; 
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void execute_command();

/* ===========================================================================
 * AUTOMATED SIMULATION & TESTING MATRIX (Exploit Injection Sandbox)
 * ===========================================================================
 */
void run_automated_testing_suite() {
    draw_string("[SIM] Initiating Core Subsystem Tests...", 45, 50, BLUE);
    
    // Test 1: Virtual RAM File System Integrity Check
    char* file_check = read_file("setup.inf");
    if (file_check != 0) {
        draw_string("TEST 1: VFS RAMFS MAP -> PASSED", 45, 62, 2); // Green color code
    } else {
        draw_string("TEST 1: VFS RAMFS MAP -> FAILED", 45, 62, 4); // Red color code
    }

    // Test 2: Auto-Type Terminal Script Exploitation Injection
    draw_string("TEST 2: SIMULATING EXPLOIT INPUT...", 45, 74, BLACK);
    
    // Forcefully inject forbidden "sudo" command array into memory buffer
    shell_buffer[0] = 's'; shell_buffer[1] = 'u'; shell_buffer[2] = 'd'; shell_buffer[3] = 'o';
    shell_buffer[4] = '\0';
    
    // Busy loop delay emulation before execution payload drops
    for(volatile int i = 0; i < 9000000; i++);

    // TRIGGER DOOMSDAY MACHINE INTERPRETER AUTOMATICALLY
    execute_command();
}

/* ===========================================================================
 * MAIN COMMAND INTERPRETER LOGIC (The Core Execution Hub)
 * ===========================================================================
 */
void execute_command() {
    // 1. Broadcast shell command log out to the external world via Serial Network Wire
    write_serial_string("[Shell Link] Tze executed command stream: ");
    write_serial_string(shell_buffer);
    write_serial_string("\r\n");

    // Clear output logs canvas line before printing feedback results
    draw_rectangle(45, SHELL_Y + 15, 230, 20, LIGHT_GRAY);

    // 2. Route instruction sets through the lookup evaluation matrices
    
    // Secret ARG Path: If user finds the hidden hex code inside README.md raw view
    if (strcmp(shell_buffer, "exec_cmd_bozo_999") == 0) {
        draw_string("ARG CODE VALIDATED. ACCESS GRANTED.", 45, SHELL_Y + 15, 2); // Green
    }
    // Hardcore Path A: Trigger Kernel Panic from direct unauthorized root attempt
    else if (strcmp(shell_buffer, "sudo") == 0 || strcmp(shell_buffer, "rm -rf") == 0) {
        linenix_panic("0xDEADBEEF", "SECURITY EXPLOIT TRAPPED VIA SHELL PIPELINE.");
    }
    // Hardcore Path B: Failed ARG code sequence entry
    else if (strcmp(shell_buffer, "bozo") == 0) {
        linenix_panic("0x000003E7", "EXEC_CMD INTEGRITY FAILURE (BAD CIPHER).");
    }
    // Standard command options
    else if (strcmp(shell_buffer, "
