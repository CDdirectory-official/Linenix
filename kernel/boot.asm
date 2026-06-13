; ==============================================================================
; File Path: kernel/boot.asm
; Description: 16-bit MBR Bootloader that triggers VGA Mode 13h and switches to PM.
; ==============================================================================
[org 0x7c00]
[bits 16]

start:
    ; Force bios video card to render VGA Mode 13h (320x200, 256 colors)
    mov ah, 0x00
    mov al, 0x13
    int 0x10

    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:init_pm

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern _start
    jmp _start

gdt_start:
gdt_null: dd 0x0, 0x0
gdt_code: dw 0xffff, 0x0, db 0x0, db 10011010b, db 11001111b, db 0x0
gdt_data: dw 0xffff, 0x0, db 0x0, db 10010010b, db 11001111b, db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 510-($-$$) db 0
dw 0xaa55