/*
 * helper.h - Miscellaneous functions for integers and strings.
 *
 * Author: Philip R. Simonson
 * Date  : 07/04/2020
 *
 ********************************************************************
 */

#include "helper.h"
#include "timer.h"

/* ------------------------ Used internally ---------------------- */

char _itoa[32];
const char _hexdigits[] = {
	'0', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', 'A', 'B', 'C', 'D',
	'E', 'F'
};

/* ----------------------- Helper Functions ---------------------- */

/* Get length of given string.
 */
int strlen(const char *s)
{
	int i = 0;
	while(s[i] != 0) i++;
	return i;
}
/* Strip off new line from string.
 */
int strstrip(char *s)
{
	int pos = strlen(s);
	while(--pos >= 0)
		if(s[pos] == '\n')
			s[pos] = 0;
	return pos;
}
/* Compares two strings together.
 */
int strcmp(const char *s, const char *t)
{
	int i;
	for(i = 0; s[i] == t[i]; i++)
		if(s[i] == 0)
			return 0;
	return s[i]-t[i];
}

// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
 
// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base, int size)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }
 
    // consider the absolute value of the number
	int n = value;
    if (value < 0) {
		n *= -1;
	}
 
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
 
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
 
    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
 
    buffer[i] = '\0'; // null terminate string
 
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

/* Convert integer to c-string safely.
 */
void itoa_s(int n, unsigned base, char *buf, int size)
{
	if(base > 16) return;
	if(n < 0) {
		buf[0] = '-';
		buf[1] = 0;
	}
	itoa(n, buf, base, size);
}

/* ----------------------- Helper Functions ---------------------- */

/* Sleep for a specific number of ticks.
 */
void delay(unsigned int ticks)
{
	unsigned int eticks = get_timer_ticks()+ticks;
	while(get_timer_ticks() < eticks);
}
/* Sleep for a specific number of seconds.
 */
void sleep(unsigned int seconds)
{
	unsigned int eseconds = get_timer_seconds()+seconds;
	while(get_timer_seconds() < eseconds);
}
/* Append to kernel buffer.
 */
void append(char *s, char c)
{
	int len = strlen(s);
	s[len++] = c;
	s[len] = '\0';
}
/* Backspace from the kernel buffer.
 */
void backspace(char *s)
{
	int len = strlen(s);
	s[len-1] = '\0';
}
