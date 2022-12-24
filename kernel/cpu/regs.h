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

#endif
