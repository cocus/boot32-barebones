/*
 * kernel.c - Source file for kernel, main operation source code.
 *
 * Author: Philip R. Simonson
 * Date  : 06/27/2020
 *
 ***********************************************************************
 */

#include <stdbool.h>

#include "kernel.h"
#include "isr.h"
#include "gdt.h"
#include "vga.h"
#include "helper.h"
#include "io.h"
#include "shell.h"
#include "keyboard.h"
#include "timer.h"
#include "system.h"

#include "task.h"

#define isascii(c) ((unsigned)(c) <= 0x7F)
#define MAXBUF 100

bool kbd_istyping;
bool login_active;

/* Get key from keyboard.
 */
int getch(void)
{
	KEYCODE key = KEY_UNKNOWN;
	
	while(key == KEY_UNKNOWN)
		key = kbd_get_last_key();
	kbd_discard_last_key();
	return key;
}
/* Get command from user and put to buffer.
 */
void get_command(char *buf, int size)
{
		KEYCODE key;
		bool buf_char;
		int i = 0;

		while(i < size) {
			buf_char = true;
			key = getch();

			if(key == KEY_RETURN) {
				kputc('\n');
				kbd_istyping = false;
				break;
			}

			if(key == KEY_BACKSPACE) {
				if(i > 0) {
					buf[i] = '\0';
					kputc('\b');
					--i;
				}
				continue;
			}

			// Update if keyboard is typing or not.
			kbd_istyping = buf_char;

			if(buf_char) {
				if(isascii(key)) {
					char c = kbd_key_to_ascii(key);
					buf[i++] = c;
					kputc(c);
				}
			}

			delay(3);
		}
		buf[i] = '\0';
}

char task_buffer[512] = { 0 };


void my_task(void)
{
	u32_t last_ticks = 0;
	kprintf("%s: hello!\n", __func__);

	for(;;) {
		if (last_ticks != get_timer_ticks()) {
			if ((get_timer_ticks() % 5) == 0) {
				kprintf("%s: ticks %d\n", __func__, get_timer_ticks());
			}
			last_ticks = get_timer_ticks();
		}
		sleep(5);
	}
}

/* Entry point for kernel.
 */
void kernel_main(void)
{
	/*const unsigned short snd[] = {500, 1000, 3000, 1500, 800};
	const int tsnd = sizeof(snd)/sizeof(snd[0]);*/
	char key_buffer[MAXBUF];

	// Initialize the variables.
	kbd_istyping = false;
	login_active = true;

	tasks_init();
	// Initialize the terminal and install ISRs and IRQs.
	term_init(BLACK, WHITE);
	gdt_install();
	isr_install();
	irq_install();

	// Display loading message and play music.
	//kprintf("Loading system! Please wait");
	/*for(int i = 0; i < tsnd; i++) {
		sound(snd[i]);
		delay(5);
		sound(0);
		sleep(1);
		kputc('.');
	}*/
	sound(0);
	//clear_screen();
	
	// Display welcome message to user and prompt.
	//kprintf(WELCOME_MESSAGE);
	kprintf("on %s()\n", __func__ );

	kprintf("task_buffer at 0x%x\n", task_buffer);


	regs_t regs = { 0 };
	READ_CURRENT_REGS(&regs);
	kprintf("EAX: 0x%x %d\nEBX: 0x%x %d\nECX: 0x%x %d\nEDX: 0x%x %d\nESI: 0x%x %d\nEDI: 0x%x %d\nESP: 0x%x %d, SS: 0x%x %d, ES: 0x%x %d, DS: 0x%x %d, CS: 0x%x %d, EBP: 0x%x %d\n",
		regs.eax, regs.eax, regs.ebx, regs.ebx, regs.ecx, regs.ecx, regs.edx, regs.edx, regs.esi, regs.esi, regs.edi, regs.edi, regs.esp, regs.esp, regs.ss, regs.ss, regs.es, regs.es, regs.ds, regs.ds, regs.cs, regs.cs, regs.ebp, regs.ebp);


	tasks_add_new((void*)my_task, "my_task", &task_buffer[sizeof(task_buffer)], NULL);
	/*u32_t last = get_timer_ticks();

	while(1)
	{
		if (last != get_timer_ticks())
		{
			last = get_timer_ticks();
			kprintf("New ticks: %d\n", last);
		}
	}*/

	for(;;) {
		get_command(key_buffer, sizeof(key_buffer));
		process_command(key_buffer);
		kprintf("> ");
	}
}

