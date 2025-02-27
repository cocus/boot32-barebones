/*
 * vga.c - Source file containing a simple VGA video driver.
 *
 * Author: Philip R. Simonson
 * Date  : 06/27/2020
 *
 ********************************************************************
 */
 
#include <stdbool.h>

#include "vga.h"
#include "ports.h"
#include "util.h"
#include "helper.h"

static unsigned char _text_attr = 0, _term_init = 0;
unsigned char *vga_buffer = NULL;

/* ------------------------- Private Functions -------------------------- */

/* Get offset for screen.
 */
int get_screen_offset(int col, int row)
{
	return 2 * (row * MAX_COLS + col);
}
/* Get offset for rows.
 */
int get_offset_row(int offset)
{
	return (offset / (2 * MAX_COLS));
}
/* Get offset for columns.
 */
int get_offset_col(int offset)
{
	return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}
/* Get cursor position on the screen.
 */
int get_cursor_offset(void)
{
	outb(REG_VGA_CTRL, 14);
	int offset = inb(REG_VGA_DATA) << 8;
	outb(REG_VGA_CTRL, 15);
	offset += inb(REG_VGA_DATA);
	return offset * 2; /* Position * size of character cell */
}
/* Set cursor position on the screen.
 */
void set_cursor_offset(int offset)
{
	offset /= 2;
	outb(REG_VGA_CTRL, 14);
	outb(REG_VGA_DATA, (unsigned char)(offset >> 8));
	outb(REG_VGA_CTRL, 15);
	outb(REG_VGA_DATA, (unsigned char)offset);
}

/* ------------------------- Public Funcitons  --------------------------- */

/* Make text attribute from foreground and background.
 */
unsigned char make_attr(unsigned char bg, unsigned char fg)
{
	return ((bg << 4) | fg);
}
/* Set color text attribute.
 */
void set_text_attr(unsigned char bg, unsigned char fg)
{
	_text_attr = make_attr(bg, fg);
}
/* Put a character on the screen at given location.
 */
int print_char(int col, int row, char c)
{
//	extern char key_buffer[]; // Defined inside keyboard.c
	extern bool kbd_istyping; // Defined inside keyboard.c
	static int last_col = 0;
	int offset;

	// Initialize vga for terminal if not already initialized
	if(!_term_init) term_init(BLUE, YELLOW);

	// Get current offset or rows and columns, plus offset
	if(col >= 0 && row >= 0) {
		offset = get_screen_offset(col, row);
	} else {
		offset = get_cursor_offset();
		row = get_offset_row(offset);
		col = get_offset_col(offset);
	}

	// Handle line breaks, backspace and normal characters
	if(c == '\n') {
		row = get_offset_row(offset);
		offset = get_screen_offset(0, row+1);
	} else if(c == '\b') {
		if((row == 0 && col > 0) || (col >= 0 && last_col != 0)) { // Bounds check for shell
			offset -= 2;
			vga_buffer[offset] = ' ';
			vga_buffer[offset+1] = _text_attr;
		} else {
			return offset;
		}
	} else {
		vga_buffer[offset] = c;
		vga_buffer[offset+1] = _text_attr;
		offset += 2;
		if(!kbd_istyping)
			last_col = get_offset_col(offset)+1;
	}

	// Scroll the screen up
	if(offset >= MAX_ROWS*MAX_COLS*2) {;
		for(int i = 0; i < MAX_ROWS; i++) {
			memcpy((char*)get_screen_offset(0, i-1)+VGA_ADDRESS,
				(char*)get_screen_offset(0, i)+VGA_ADDRESS, MAX_COLS*2);
		}
		for(int i = 0; i < MAX_COLS; i++) {
			offset = get_screen_offset(i, MAX_ROWS-1);
			vga_buffer[offset] = ' ';
			vga_buffer[offset+1] = _text_attr;
			offset += 2;
		}
		offset -= MAX_COLS*2;
	}

	// Set cursor to current offset and return offset
	set_cursor_offset(offset);
	return offset;
}
/* Initialize the terminal (graphics).
 */
void clear_screen(void)
{
	int col = 0, row = 0;

	if(!_term_init) term_init(BLUE, YELLOW);

	for(int x = 0; x < MAX_COLS; x++) {
		for(int y = 0; y < MAX_ROWS; y++) {
			int offset = print_char(col, row, ' ');
			row = get_offset_row(offset);
			col = get_offset_col(offset);
		}
	}

	set_cursor_offset(get_screen_offset(0, 0));
}
/* Terminal initialization.
 */
void term_init(unsigned char bg, unsigned char fg)
{
	if(!_term_init) {
		_term_init = 1;
		_text_attr = make_attr(bg, fg);
		vga_buffer = (void*)VGA_ADDRESS;
		//clear_screen();
	}
}
