#include "types.h"
#include "regs.h"
#include "stdbool.h"
#include "isr.h"

typedef struct {
    u32_t id;
    char name[16];
    u32_t ticks;

    bool used; // remove this crap, make a linked list
    registers_t last_context;
} task;


void tasks_init(void);
void tasks_add_new(void* fn, char* name, void* stack, task * t);
void tasks_timer_tick(registers_t * regs);
