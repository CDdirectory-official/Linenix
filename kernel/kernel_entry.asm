; ==============================================================================
; kernel_entry.asm - Low-Level Interrupt Setup & Kernel Jumper
; Remaps the PIC, builds a minimal IDT for Keyboard (IRQ 1), and jumps to C.
; ==============================================================================

[bits 32]
[global _start]
[extern kernel_main]        ; Link ke fungsi utama di kernel_graphics.c
[global keyboard_handler]   ; Biar bisa di-load sama IDT
[extern keyboard_handler_c] ; Link ke driver keyboard C lo

_start:
    cli                     ; 1. Matikan interupsi dulu pas setup

    call remap_pic          ; 2. Remap PIC (Biar IRQ Keyboard gak tabrakan ama CPU Fault)
    call setup_idt          ; 3. Daftarin fungsi keyboard ke memori IDT
    
    lidt [idt_descriptor]   ; 4. Load IDT Register ke CPU
    sti                     ; 5. Nyalain lagi interupsi (Keyboard turns ON!)

    call kernel_main        ; 6. Terbang langsung ke visual GUI C lo
    jmp $                   ; Freeze kalau kernel_main return

; ------------------------------------------------------------------------------
; KEYBOARD INTERRUPT WRAPPER
; ------------------------------------------------------------------------------
keyboard_handler:
    pusha                   ; Simpan semua register CPU biar gak korup
    call keyboard_handler_c ; Panggil driver keyboard C lo!
    popa                    ; Kembalikan register CPU ke semula
    iret                    ; Interrupt Return (Selesai melayani interupsi)

; ------------------------------------------------------------------------------
; PROGRAMMABLE INTERRUPT CONTROLLER (PIC) REMAP
; ------------------------------------------------------------------------------
remap_pic:
    ; Kirim ICW1 (Initialization command) ke PIC Utama & Tambahan
    mov al, 0x11
    out 0x20, al
    out 0xA0, al

    ; Kirim ICW2 (Vector Offset): Remap IRQ 0-7 ke Interrupt 0x20-0x27
    mov al, 0x20            ; IRQ 1 (Keyboard) sekarang jadi Interrupt 0x21!
    out 0x21, al
    mov al, 0x28
    out 0xA1, al

    ; Kirim ICW3 (Wiring cascading)
    mov al, 0x04
    out 0x21, al
    mov al, 0x02
    out 0xA1, al

    ; Kirim ICW4 (8086/88 Mode)
    mov al, 0x01
    out 0x21, al
    out 0xA1, al

    ; Mask semua interupsi KECUALI IRQ 1 (Keyboard)
    mov al, 0xFD            ; 0xFD = 11111101b (Cuma bit 1/Keyboard yang ON)
    out 0x21, al
    mov al, 0xFF            ; Matikan semua IRQ di PIC tambahan
    out 0xA1, al
    ret

; ------------------------------------------------------------------------------
; INTERRUPT DESCRIPTOR TABLE (IDT) SETUP
; ------------------------------------------------------------------------------
setup_idt:
    ; Kita daftarin keyboard_handler ke Interrupt 0x21 (Offset: 0x21 * 8 = 168)
    mov eax, keyboard_handler
    
    ; Taruh bit-bit alamat fungsi assembly ke struktur IDT Entry 0x21
    mov word [idt_table + 168], ax        ; Base Low (Bits 0-15)
    mov word [idt_table + 170], 0x08      ; Kernel Code Segment Selector
    mov byte [idt_table + 172], 0         ; Reserved byte
    mov byte [idt_table + 173], 0x8E      ; Access Flags (Present, Ring 0, 32-bit Interrupt Gate)
    shr eax, 16
    mov word [idt_table + 174], ax        ; Base High (Bits 16-31)
    ret

; ------------------------------------------------------------------------------
; IDT MEMORY DATA STRUCTURES
; ------------------------------------------------------------------------------
align 4
idt_descriptor:
    dw (256 * 8) - 1        ; Limit: Ukuran IDT (256 entry * 8 byte per entry)
    dd idt_table            ; Base Address: Alamat memori tabel IDT

idt_table:
    times 256 * 8 db 0      ; Sediakan ruang kosong 2048 byte di RAM buat tabel IDT