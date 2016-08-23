#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Check if the compiler thinks we are targeting the wrong OS.
#if defined(__linux__)
	#error "You are not using a cross-compiler, you will most certainly run into trouble."
#endif

// This kernel will only work for 32-bit x86 targets
#if !defined(__i386__)
	#error "This kernel needs to be compiled with an ix86-elf compiler"
#endif

// Hardware text mode color constants
enum vga_color {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color) {
	uint16_t char16 = c;
	uint16_t color16 = color;
	return char16 | color16 << 8;
}

size_t strlen(const char * str) {
	size_t ret = 0;
	while (str[ret] != 0) {
		ret++;
	}
	return ret;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t * terminal_buffer;

void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = (uint16_t *)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = (y * VGA_WIDTH) + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}

void terminal_set_color(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = (y * VGA_WIDTH) + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_scroll_line() {
	for (size_t row = 0; row < VGA_HEIGHT; row++) {
		for (size_t col = 0; col < VGA_WIDTH; col++) {
			const size_t index = (row * VGA_WIDTH) + col;
			terminal_buffer[index] = terminal_buffer[index + VGA_WIDTH];
		}
	}
	terminal_row--;
}

void terminal_putchar(char c) {
	if (c == '\n') {
		terminal_row++;
		terminal_column = 0;
	} else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column >= VGA_WIDTH) {
			terminal_column = 0;
		}
	}

	if (terminal_row >= VGA_HEIGHT) {
		terminal_scroll_line();
	}
}

void terminal_writestring(const char * data) {
	size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; i++) {
		terminal_putchar(data[i]);
	}
}

void print_header() {
	
	terminal_color = make_color(COLOR_WHITE, COLOR_BLACK);
	terminal_writestring("Hola Mundo, Grupo Elementary OS\n");
	terminal_writestring("Proyecto Sistemas Operativos v1.0");
	terminal_writestring("\n");
	terminal_color = make_color(COLOR_GREEN, COLOR_BLACK);
	terminal_writestring("_______________________________________________________________________________________");
	
}


void kernel_main() {
	terminal_initialize();
	print_header();	
	
}