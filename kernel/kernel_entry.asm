; ==============================================================================
; File Path: kernel/kernel_entry.asm
; Description: Maps the PIC vectors and binds the C keyboard interrupt logic.
; ==============================================================================
[bits 32]
[global _start]
[extern kernel_main]
[global keyboard_handler]
[extern keyboard_handler_c]

_start:
    cli
    call remap_pic
    call setup_idt
    lidt [idt_descriptor]
    sti
    call kernel_main
    jmp $

keyboard_handler:
    pusha
    call keyboard_handler_c
    popa
    iret

remap_pic:
    mov al, 0x11
    out 0x20, al
    out 0xA0, al
    mov al, 0x20 ; Map Keyboard IRQ 1 to Interrupt vector 0x21
    out 0x21, al
    mov al, 0x28
    out 0xA1, al
    mov al, 0x04
    out 0x21, al
    mov al, 0x02
    out 0xA1, al
    mov al, 0x01
    out 0x21, al
    out 0xA1, al
    mov al, 0xFD ; Mask all IRQs except Keyboard (IRQ 1)
    out 0x21, al
    mov al, 0xFF
    out 0xA1, al
    ret

setup_idt:
    mov eax, keyboard_handler
    mov word [idt_table + 168], ax
    mov word [idt_table + 170], 0x08
    mov byte [idt_table + 172], 0
    mov byte [idt_table + 173], 0x8E
    shr eax, 16
    mov word [idt_table + 174], ax
    ret

align 4
idt_descriptor:
    dw (256 * 8) - 1
    dd idt_table

idt_table:
    times 256 * 8 db 0
