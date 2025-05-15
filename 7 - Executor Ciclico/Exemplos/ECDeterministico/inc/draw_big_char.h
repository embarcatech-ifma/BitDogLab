#ifndef DRAW_BIG_CHAR_H
#define DRAW_BIG_CHAR_H

#include <stdbool.h>
#include <stdint.h>
#include "ssd1306.h"

// Desenha um caractere grande no buffer ssd[] a partir de bitmap de 64 bytes
void draw_big_char(uint8_t *ssd, int x, int y, const uint8_t *bitmap) {
    for (int row = 0; row < 32; row++) {
        for (int col = 0; col < 16; col++) {
            int byte_index = (row * 2) + (col / 8);
            bool pixel = (bitmap[byte_index] >> (7 - (col % 8))) & 0x01;
            ssd1306_set_pixel(ssd, x + col, y + row, pixel);
        }
    }
}
#endif