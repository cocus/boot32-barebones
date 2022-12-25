/*
 * regs.h - Simple implementation for register structure.
 *
 * Author: Philip R. Simonson
 * Date: 12/20/2020
 *
 ************************************************************************
 */

#ifndef _REGS_H_
#define _REGS_H_

#include "types.h"

/* 32 bit registers. */
typedef struct tagREG32 {
	u32_t eax, ebx, ecx, edx;
	u32_t esi, edi, ebp, esp;
	u32_t cs, ss, ds, es, cflag, eflag;
} regs_t;

#define READ_CURRENT_REGS(to) \
{    __asm__ __volatile__( \
        "" \
        : "=a"((to)->eax), "=b"((to)->ebx), "=c"((to)->ecx), "=d"((to)->edx), \
        "=S"((to)->esi), "=D"((to)->edi) \
    ); \
}


/* A saved stack frame is completely defined by a pointer to the top of the
 * stack frame. */
typedef void *frame_t;

/* The action performed for a new frame takes two arguments: the context
 * argument passed to create_frame() when this frame was first established and
 * the argument passed to the first activating switch_frame() call.
 *
 * This routine must never return. */
typedef __attribute__((noreturn))
    void (*frame_action_t)(void *arg, void *context);

/* Switch to new_frame, previously established by create_frame() or an earlier
 * switch_frame().  The caller's stack frame is written to *old_frame. */
void * switch_frame(frame_t *old_frame, frame_t new_frame, void *arg);

#endif
