/*
 * File Path: kernel/scheduler.h
 * Description: Lightweight Preemptive Task Scheduler for Linenix Kernel.
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_TASKS 2

typedef struct {
    unsigned int esp;       # Saved Stack Pointer for the task
    unsigned int is_running;
} Task_Control_Block;

static Task_Control_Block task_list[MAX_TASKS];
static int current_task_index = 0;

# External pixel rendering routine
extern void draw_rectangle(int x, int y, int width, int height, unsigned char color);

/**
 * Task A: Dummy background processing unit 1 (Flashes a small Green square)
 */
void background_task_A() {
    while(1) {
        draw_rectangle(280, 5, 5, 5, 2); # Green Blink
        for(volatile int i = 0; i < 50000; i++); # Tiny Delay
        draw_rectangle(280, 5, 5, 5, 8); # Dark Gray Reset
        for(volatile int i = 0; i < 50000; i++);
    }
}

/**
 * Task B: Dummy background processing unit 2 (Flashes a small Orange square)
 */
void background_task_B() {
    while(1) {
        draw_rectangle(290, 5, 5, 5, 43); # Orange Blink
        for(volatile int i = 0; i < 50000; i++);
        draw_rectangle(290, 5, 5, 5, 8);  # Dark Gray Reset
        for(volatile int i = 0; i < 50000; i++);
    }
}

/**
 * Central Scheduler Dispatcher called by the PIT Timer Interrupt (IRQ 0)
 * Alternates the CPU execution stack frame between Task A and Task B.
 */
unsigned int linenix_schedule(unsigned int current_esp) {
    if (task_list[0].is_running == 0) {
        # Bootstrap task tracking blocks on the first interrupt pass
        task_list[0].esp = current_esp;
        task_list[0].is_running = 1;
        task_list[1].is_running = 1;
        return current_esp;
    }

    # Save the stack pointer of the task that just got interrupted
    task_list[current_task_index].esp = current_esp;

    # Context Switch: Toggle between Task 0 and Task 1
    current_task_index = (current_task_index + 1) % MAX_TASKS;

    # Return the target stack pointer for the next task to the Assembly handler
    return task_list[current_task_index].esp;
}

#endif
