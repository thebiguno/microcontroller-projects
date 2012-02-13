#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

/*
 * Draws a byte[] framebuffer onto 8x8 matrix.  The framebuffer must be at least 8-bytes long.
 */
void matrix_draw(uint8_t red[], uint8_t green[]);

#endif
