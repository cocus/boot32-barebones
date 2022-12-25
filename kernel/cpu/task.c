#include "system.h"
#include "task.h"
#include "util.h"
#include "helper.h"
#include "io.h"

static u32_t _tsk_count = 0;
static task _tasks[32];

static task* _current_task = NULL;

static u32_t _task_ticks = 0;

void tasks_timer_tick(registers_t * regs)
{
    if (++_current_task->ticks == 69) {
        // store current context
        memcpy(&_current_task->last_context, regs, sizeof(_current_task->last_context));
        _current_task->ticks = 0;

        // switch to new
        u32_t i;
        for (i = 0; i < sizeof(_tasks)/sizeof(_tasks[0]); ++i) {
            if (_tasks[i].id == _current_task->id) {
                continue;
            }
            if (!_tasks[i].used) {
                continue;
            }

            kprintf("tsk %d, switching to %d!\n", _current_task->id, _tasks[i].id);

            // this task looks good to switch to
            memcpy(regs, &_tasks[i].last_context, sizeof(_current_task->last_context));
            /*regs->eax = _tasks[i].last_context.eax;
            regs->ebx = _tasks[i].last_context.ebx;
            regs->ecx = _tasks[i].last_context.ecx;
            regs->edx = _tasks[i].last_context.edx;

            regs->ebp = _tasks[i].last_context.ebp;
            regs->esi = _tasks[i].last_context.esi;
            regs->eflags = _tasks[i].last_context.eflags;


            regs->eip = _tasks[i].last_context.eip;
            regs->esp = _tasks[i].last_context.esp;*/
            _current_task = &_tasks[i];
            break;
        }
    }
}

void tasks_init(void)
{
    disable(); // should be enter_critical();

    u32_t i;
    for (i = 0; i < sizeof(_tasks)/sizeof(_tasks[0]); ++i) {
        _tasks[i].id = i;
        memset(&_tasks[i].last_context, 0, sizeof(_tasks[i].last_context));
        memset(_tasks[i].name, 0, sizeof(_tasks[i].name));
        _tasks[i].ticks = 0;
        _tasks[i].used = false;
    }

    _tasks[0].used = true;
    memcpy(_tasks[0].name, "kernel", 7);

    _current_task = &_tasks[0];

    _tsk_count = 1;

    enable(); // should be leave_critical();
}

void tasks_add_new(void* fn, char* name, void* stack, task * t)
{
    disable(); // should be enter_critical();

    u32_t i;
    for (i = 0; i < sizeof(_tasks)/sizeof(_tasks[0]); ++i) {
        if (!_tasks[i].used) {
            memset(&_tasks[i].last_context, 0, sizeof(_tasks[i].last_context));
            memcpy(_tasks[i].name, name, strlen(name));

            _tasks[i].last_context.ss = 0x10;
            _tasks[i].last_context.cs = 0x10;
            _tasks[i].last_context.ds = 0x10;
            _tasks[i].last_context.eip = (u32_t)fn;
            _tasks[i].last_context.esp = (u32_t)stack;

            _tasks[i].ticks = 0;
            _tasks[i].used = true;

            if (t) {
                t = &_tasks[i];
            }

            enable(); // should be leave_critical();
            return;
        }
    }

    enable(); // should be leave_critical();
}
