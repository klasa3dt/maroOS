#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//0xE0, 0x48 	cursor up pressed
//0xE0, 0x50 	cursor down pressed
#define PS2_DATA_PORT 0x60

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

static const size_t vga_width = 80;
static const size_t vga_height = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < vga_height; y++) {
        for (size_t x = 0; x < vga_width; x++) {
            const size_t index = y * vga_width + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * vga_width + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a" (value), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

uint8_t ps2_read_data(void) {
    uint8_t scancode;
    asm volatile("inb %1, %0" : "=a" (scancode) : "Nd" (PS2_DATA_PORT));
    return scancode;
}

uint16_t get_cursor_position(void)
{
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void move_cursor(int x, int y)
{
	uint16_t pos = y * vga_width + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_scrolling(uint8_t scancode, uint8_t cursor_position) {

    int cursor_position_y = cursor_position / vga_width;
    int cursor_position_x = cursor_position % vga_width;

    //cursor arrow up zostalo klikniete
    if (scancode == 0x48) {
        for (size_t y = 1; y < vga_height; y++) {
            for (size_t x = 0; x < vga_width; x++) {
                const size_t src_index = cursor_position_y * vga_width + cursor_position_x;
                const size_t dst_index = (cursor_position_y + 1) * vga_width + cursor_position_x;
                terminal_buffer[dst_index] = terminal_buffer[src_index];
            }
        }
    }
    else if (scancode == 0x50) {
        for (size_t y = 1; y < vga_height; y++) {
            for (size_t x = 0; x < vga_width; x++) {
                const size_t src_index = cursor_position_y * vga_width + cursor_position_x;
                const size_t dst_index = (cursor_position_y - 1) * vga_width + cursor_position_x;
                terminal_buffer[dst_index] = terminal_buffer[src_index];
            }
        }
    }
}


void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == vga_height) {
            for (size_t y = 1; y < vga_height; y++) {
                for (size_t x = 0; x < vga_width; x++) {
                    const size_t src_index = y * vga_width + x;
                    const size_t dst_index = (y - 1) * vga_width + x;
                    terminal_buffer[dst_index] = terminal_buffer[src_index];
                }
            }
            //pusta linia na koncu terminala
            for (size_t x =  0; x < vga_width; x++) {
                const size_t index = (vga_height - 1) * vga_width + x;
                terminal_buffer[index] = vga_entry(' ', terminal_color);
            }
            //ustawienie kursora na ostatni wiersz
            terminal_row = vga_height - 1;
        }
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == vga_width) {
            terminal_column = 0;
            if (++terminal_row == vga_height) {
                for (size_t y = 1; y < vga_height; y++) {
                for (size_t x = 0; x < vga_width; x++) {
                    const size_t src_index = y * vga_width + x;
                    const size_t dst_index = (y - 1) * vga_width + x;
                    terminal_buffer[dst_index] = terminal_buffer[src_index];
                }
            }
            for (size_t x = 0; x < vga_width; x++) {
                const size_t index = (vga_height - 1) * vga_width + x;
                terminal_buffer[index] = vga_entry(' ', terminal_color);
            }
            terminal_row = vga_height - 1;
        }
        }
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}


void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

void main(void) {

    terminal_initialize();
    terminal_writestring("Hello, this is niggerOS\n");
    for (int i = 0; i < 28; i++) {
        terminal_writestring("kill yourself nigger\n");
    };

    while (1) {
        uint8_t scancode;

        scancode = ps2_read_data();
        
        terminal_scrolling(scancode, get_cursor_position());
    }
}