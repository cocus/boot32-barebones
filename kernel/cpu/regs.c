#include "regs.h"

__asm__(
"       .text\n"

// void * switch_frame(frame_t *old_frame, frame_t new_frame, void *arg)
"switch_frame:\n"

// On entry have following arguments on stack:
//   4(%esp)     address of frame to be written
//   8(%esp)     frame to be loaded
//   12(%esp)    argument to pass through switch
        // Pick up the arguments
"       movl    4(%esp), %ecx\n"       // %ecx = old_frame
"       movl    8(%esp), %edx\n"       // %edx = new_frame
"       movl    12(%esp), %eax\n"      // %eax = arg = result register

        // Save registers ABI requires to be preserved.
"       pushl   %ebp\n"
"       pushl   %ebx\n"
"       pushl   %edi\n"
"       pushl   %esi\n"

        // Save SIMD and floating point state just in case this coroutine has
        // made a change: we'll make such changes local to each coroutine.
"       sub     $4, %esp\n"
"       stmxcsr (%esp)\n"
"       sub     $4, %esp\n"
"       fstcw   (%esp)\n"

        // Switch stack frames.
"       movl    %esp, (%ecx)\n"
"       movl    %edx, %esp\n"

        // Restore saved floating point and SIMD state.
"       fnclex\n"
"       fldcw   (%esp)\n"
"       add     $4, %esp\n"
"       ldmxcsr (%esp)\n"
"       add     $4, %esp\n"

        // Restore previously saved registers and we're done, the result is
        // already in the right place.
"       popl    %esi\n"
"       popl    %edi\n"
"       popl    %ebx\n"
"       popl    %ebp\n"
"       ret\n"

);
