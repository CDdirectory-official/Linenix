/*
 * Your Core Kernel & Input Driver
 * High-performance, zero-bloat hardware interfacing.
 */

#define VIDEO_MEMORY (char*)0xB8000
#define WHITE_ON_BLACK 0x07
#define GREEN_ON_BLACK 0x0A

/* Helper function to read a byte from a hardware port */
unsigned char inb(unsigned short port) {
    unsigned char result;
    // Direct hardware I/O communication with the CPU registers
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void print_string(char* str, char attribute_color) {
    static int screen_pointer = 0; 
    int i = 0;
    int bytes_per_line = 160; 

    while (str[i] != '\0') {
        if (str[i] == '\n') {
            int current_line = screen_pointer / bytes_per_line;
            screen_pointer = (current_line + 1) * bytes_per_line;
            i++;
            continue;
        }
        VIDEO_MEMORY[screen_pointer] = str[i];
        VIDEO_MEMORY[screen_pointer + 1] = attribute_color;
        screen_pointer += 2;
        i++;
    }
}

/* * Keyboard Driver: Handles keypress events from Port 0x60
 * Scancode 0x1E is 'A', 0x30 is 'B', etc.
 */
void keyboard_handler_c() {
    unsigned char scancode = inb(0x60);

    // If the highest bit is set, it means the key was released. We only care about press events.
    if (scancode & 0x80) {
        // Key released - do nothing
    } else {
        // Simple scancode mapping table for testing
        if (scancode == 0x1E) print_string("A", WHITE_ON_BLACK);
        else if (scancode == 0x30) print_string("B", WHITE_ON_BLACK);
        else if (scancode == 0x2E) print_string("C", WHITE_ON_BLACK);
        // You can expand this mapping table for the entire alphabet later, bozo.
    }

    // Send EOI (End of Interrupt) signal to the PIC (Programmable Interrupt Controller)
    // This tells the hardware we are ready to accept the next keypress!
    __asm__("mov $0x20, %al; out %al, $0x20");
}

void kernel_main() {
    print_string("Welcome to your own kernel!\n", WHITE_ON_BLACK);
    print_string("[SUCCESS] Keyboard driver active. Try pressing keys inside QEMU.\n", GREEN_ON_BLACK);
    
    // The kernel will now sit here and wait for keyboard interrupts to trigger
    while(1); 
}

/* Call this function when everything goes wrong */
void kernel_panic(char* error_message) {
    // Fill the whole line with red attribute to block the screen
    print_string("\n!!!!!!!!!!!!!!!! KERNEL PANIC !!!!!!!!!!!!!!!!\n", 0x0C); // 0x0C is Red
    print_string("ERROR: ", 0x0C);
    print_string(error_message, 0x0C);
    print_string("\nSystem halted. Please restart your VM, bozo.\n", 0x0C);
    
    // Freeze the CPU completely
    while(1);
}
