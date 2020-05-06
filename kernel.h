#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

#define NULL 0

#define VGA_ADDRESS 0xB8000
#define BUFSIZE 2200

typedef unsigned char UINT8;
typedef unsigned short UINT16;

uint16* vga_buffer;

int DIGIT_ASCII_CODES[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};


unsigned int VGA_INDEX;

UINT16* TERMINAL_BUFFER;

enum vga_color {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GREY,
    DARK_GREY,
    BRIGHT_BLUE,
    BRIGHT_GREEN,
    BRIGHT_CYAN,
    BRIGHT_RED,
    BRIGHT_MAGENTA,
    YELLOW,
    WHITE,
};


#include "keyboard.h"


#endif
