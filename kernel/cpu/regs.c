#include "regs.h"


void read_current_regs(regs_t * to)
{
    if (!to)
        return;

    __asm__ __volatile__(
        ""
        : "=a"(to->eax), "=b"(to->ebx), "=c"(to->ecx), "=d"(to->edx),
        "=S"(to->esi), "=D"(to->edi)
    );
}
