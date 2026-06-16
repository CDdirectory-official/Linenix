; ==============================================================================
; File Path: kernel/kernel_entry.asm
; Description: Low-Level Interrupt Vectoring, PIC Remapping, & Context Switching.
; Manages GDT-to-IDT traps, captures hardware IRQs, and routes central syscalls.
; ==============================================================================

[bits 32]
[global _start]

; External core linkages originating from central C subsystems
[extern kernel_main]        
[extern keyboard_handler_c] 
[extern syscall_handler_c]  
[extern linenix_schedule]

; Global symbol definitions exported for IDT mapping procedures
[global timer_handler]
[global keyboard_handler]   
[global syscall_handler]    

_start:
    cli                     ; 1. Clear Maskable Interrupts during bootstrap phase

    call remap_pic          ; 2. Remap Programmable Interrupt Controller registers
    call setup_idt          ; 3. Structure absolute address offsets inside the IDT
    
    lidt [idt_descriptor]   ; 4. Load Interrupt Descriptor Table register allocation
    sti                     ; 5. Set Maskable Interrupts (Hardware lines active!)

    call kernel_main        ; 6. Transfer control execution to Linenix C main routine
    jmp $                   ; Fail-safe kernel execution halt trapping loop

; ------------------------------------------------------------------------------
; HARDWARE INTERRUPT WRAPPER: PIT TIMER TICK (IRQ 0 / VECTOR 0x20)
; ------------------------------------------------------------------------------
timer_handler:
    pusha                   ; Save volatile general-purpose registers for current task

    push esp                ; Pass current Stack Pointer as an argument to scheduler
    call linenix_schedule   ; Invoke Linenix task dispatcher (Returns target task ESP)
    mov esp, eax            ; Context Switch: Mount the newly selected task ESP register

    mov al, 0x20            ; Send EOI (End of Interrupt) to Master PIC hardware line
    out 0x20, al

    popa                    ; Restore general-purpose registers for the new target task
    iret                    ; Return and resume execution inside the new task context

; ------------------------------------------------------------------------------
; HARDWARE INTERRUPT WRAPPER: KEYBOARD CONTROLLER (IRQ 1 / VECTOR 0x21)
; ------------------------------------------------------------------------------
keyboard_handler:
    pusha                   ; Save volatile general-purpose CPU registers
    call keyboard_handler_c ; Invoke Central C Keyboard Driver callback routine
    popa                    ; Restore general-purpose CPU registers
    iret                    ; Return from Interrupt context execution

; ------------------------------------------------------------------------------
; SOFTWARE INTERRUPT WRAPPER: CENTRAL SYSTEM CALL GATE (VECTOR 0x80)
; ------------------------------------------------------------------------------
syscall_handler:
    pusha                   ; Save volatile general-purpose CPU registers
    push esp                ; Pass current Stack Pointer as a C function argument
    call syscall_handler_c  ; Invoke Central C IPC Mailbox/Syscall dispatcher
    popa                    ; Restore general-purpose CPU registers
    iret                    ; Return from Interrupt context execution

; ------------------------------------------------------------------------------
; HARDWARE INITIALIZATION: PROGRAMMABLE INTERRUPT CONTROLLER (8259 PIC) REMAP
; ------------------------------------------------------------------------------
remap_pic:
    ; Send ICW1 (Initialization Command Word 1) to Master and Slave PICs
    mov al, 0x11
    out 0x20, al
    out 0xA0, al

    ; Send ICW2 (Vector Offsets): Remap IRQ 0-7 to map directly to Vectors 0x20-0x27
    mov al, 0x20            ; PIT Timer (IRQ 0) -> Vector 0x20, Keyboard (IRQ 1) -> Vector 0x21
    out 0x21, al
    mov al, 0x28            ; Slave PIC remapped to offset 0x28
    out 0xA1, al

    ; Send ICW3 (Master/Slave Cascading Configuration parameters)
    mov al, 0x04
    out 0x21, al
    mov al, 0x02
    out 0xA1, al

    ; Send ICW4 (Enforce 8086/88 Environment Execution Mode)
    mov al, 0x01
    out 0x21, al
    out 0xA1, al

    ; IRQ Masking Setup: Open IRQ 0 (Timer) and IRQ 1 (Keyboard)
    ; 0xFC = 11111100b (Bit 0 and Bit 1 unmasked, all other hardware peripheral lines closed)
    mov al, 0xFC            
    out 0x21, al
    mov al, 0xFF            ; Disable all peripheral IRQ lines on Slave PIC hardware
    out 0xA1, al
    ret

; ------------------------------------------------------------------------------
; DESCRIPTOR ROUTINES: INTERRUPT DESCRIPTOR TABLE (IDT) GATE REGISTRATION
; ------------------------------------------------------------------------------
setup_idt:
    ; --- TARGET 1: Map PIT Timer ISR routine into IDT Entry Vector 0x20 ---
    ; Memory Vector Calculation Offset: 0x20 * 8 bytes per descriptor = 256
    mov eax, timer_handler
    mov word [idt_table + 256], ax        ; Base Address Low (Bits 0-15)
    mov word [idt_table + 258], 0x08      ; Kernel Code Segment Selector parameter
    mov byte [idt_table + 260], 0         ; Hardware Reserved byte padding
    mov byte [idt_table + 261], 0x8E      ; Attribute Flags: Present, Ring 0, 32-bit Gate
    shr eax, 16
    mov word [idt_table + 262], ax        ; Base Address High (Bits 16-31)

    ; --- TARGET 2: Map Keyboard ISR routine into IDT Entry Vector 0x21 ---
    ; Memory Vector Calculation Offset: 0x21 * 8 bytes per descriptor = 168
    mov eax, keyboard_handler
    mov word [idt_table + 168], ax        ; Base Address Low (Bits 0-15)
    mov word [idt_table + 170], 0x08      ; Kernel Code Segment Selector parameter
    mov byte [idt_table + 172], 0         ; Hardware Reserved byte padding
    mov byte [idt_table + 173], 0x8E      ; Attribute Flags: Present, Ring 0, 32-bit Gate
    shr eax, 16
    mov word [idt_table + 174], ax        ; Base Address High (Bits 16-31)

    ; --- TARGET 3: Map Software Syscall IPC routine into IDT Entry Vector 0x80 ---
    ; Memory Vector Calculation Offset: 0x80 * 8 bytes per descriptor = 1024
    mov eax, syscall_handler
    mov word [idt_table + 1024], ax       ; Base Address Low (Bits 0-15)
    mov word [idt_table + 1026], 0x08      ; Kernel Code Segment Selector parameter
    mov byte [idt_table + 1028], 0         ; Hardware Reserved byte padding
    mov byte [idt_table + 1029], 0xEE      ; Attribute Flags: Present, Ring 3 User Accessible!
    shr eax, 16
    mov word [idt_table + 1030], ax       ; Base Address High (Bits 16-31)
    ret

; ------------------------------------------------------------------------------
; FIXED ALLOCATION MEMORY DATA STRUCTURES
; ------------------------------------------------------------------------------
align 4
idt_descriptor:
    dw (256 * 8) - 1        ; IDT Size Table Limit: 256 structural descriptors total
    dd idt_table            ; IDT Base Pointer Address referencing the memory block

idt_table:
    times 256 * 8 db 0      ; Reserve 2048 bytes of zeroed continuous block space in RAM
