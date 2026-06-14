/*
 * File Path: drivers/video/font.h
 * Description: Minimalist 8x8 Bitmap Font Render Engine for VGA Mode 13h.
 */

#ifndef FONT_H
#define FONT_H

#include "../../include/vga_io.h"

// External pixel plotting routine from kernel_graphics.c
extern void put_pixel(int x, int y, unsigned char color);

/* Minimal hardcoded 8x8 bitmap font matrix for key setup characters */
static unsigned char font_bitmap[256][8] = {
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['S'] = {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x3C},
    ['E'] = {0x7E, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x7E},
    ['T'] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    ['U'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['P'] = {0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x40},
    ['N'] = {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42},
    ['X'] = {0x42, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x42},
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['K'] = {0x42, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42, 0x42},
    [':'] = {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/**
 * Renders a single alphanumeric character onto the screen pixel by pixel.
 */
void draw_char(char c, int x, int y, unsigned char color) {
    for (int row = 0; row < 8; row++) {
        unsigned char bits = font_bitmap[(unsigned char)c][row];
        for (int col = 0; col < 8; col++) {
            // Check if the current bit is active (1) starting from the highest bit
            if (bits & (0x80 >> col)) {
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

/**
 * Draws a null-terminated string array onto the screen workspace.
 */
void draw_string(char* str, int x, int y, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        draw_char(str[i], x + (i * 8), y, color); // Advance x offset by 8 pixels per char
        i++;
    }
}

#endif