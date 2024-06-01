#include "mstd.h"

static inline uint8_t vgaEntryColor (vgaColor foreground, vgaColor background) {

    return foreground | background << 4;
}

static inline uint16_t vgaEntry (unsigned char uc, uint8_t color) {

    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen (const char* string) {

    size_t length = 0;

    while (string[length]) {
        ++length;
    }
    return length;
}

void terminalClear(Terminal *terminal) {

    terminal->row = 0;
    terminal->column = 0;

    for (size_t y = 0; y < terminal->VGA_HEIGHT; ++y) {

        for (size_t x = 0; x < terminal->VGA_WIDTH; ++x) {

            terminalPutEntryAt(terminal, ' ', terminal->color, x, y);
        }
    }
}

void terminalInitialize (Terminal* terminal, size_t VGA_WIDTH, size_t VGA_HEIGHT) {

    terminal->color = vgaEntryColor (VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    terminal->buffer = (uint16_t*) 0xB8000;
    terminal->VGA_WIDTH = VGA_WIDTH;
    terminal->VGA_HEIGHT = VGA_HEIGHT;

    terminalClear (terminal);
}

void terminalSetColor (Terminal* terminal, uint8_t color) {

    terminal->color = color;
}

void terminalPutEntryAt (Terminal* terminal, char c, uint8_t color, size_t x, size_t y) {

    const size_t index = y * terminal->VGA_WIDTH + x;
    terminal->buffer[index] = vgaEntry (c, color);
}

void terminalPutChar (Terminal* terminal, char c) {
    if (c == '\n') {

        ++terminal->row;
        terminal->column = 0;
    }
    else {

        terminalPutEntryAt(terminal, c, terminal->color, terminal->column, terminal->row);
        ++terminal->column;
    }

    if (terminal->column >= terminal->VGA_WIDTH) {

        terminal->column = 0;
        ++terminal->row;
    }

    if (terminal->row >= terminal->VGA_HEIGHT) {
        terminal->row = 0;
    }

}

void terminalWrite (Terminal* terminal, const char* data, size_t stringLength) {
    
    for (size_t i = 0; i < stringLength; ++i) {

        terminalPutChar (terminal, data[i]);
    }
}

void terminalWriteString (Terminal* terminal, const char* data) {

    terminalWrite (terminal, data, strlen(data));
}