/*
 * mm/pmm.h - Physical Memory Manager (Bare-Metal Alocator)
 * Simple sequential memory allocator for TzeOS Kernel Space.
 */

#ifndef PMM_H
#define PMM_H

// Start allocating dynamic memory at the 2MB mark to stay clear of the kernel code
#define MEMORY_START 0x200000 
#define MEMORY_END   0x4000000 // Manage up to 64MB of RAM for now

static unsigned int free_memory_pointer = MEMORY_START;

/**
 * Bare-metal equivalent of malloc().
 * Allocates a continuous block of physical memory.
 */
void* kalloc(unsigned int size) {
    // Align allocations to 4-byte boundaries for CPU efficiency
    if (size % 4 != 0) {
        size = size + (4 - (size % 4));
    }

    if (free_memory_pointer + size > MEMORY_END) {
        return (void*)0; // Out of memory, kernel panic simulation!
    }

    void* allocated_address = (void*)free_memory_pointer;
    free_memory_pointer += size; // Move the pointer forward
    return allocated_address;
}

/**
 * Reset allocator (Simple stack-like free operation for internal resets)
 */
void kfree_all() {
    free_memory_pointer = MEMORY_START;
}

#endif