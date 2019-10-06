#include "Matrix.h"
#include "ph42121rgb16s.h"

using namespace digitalcave;

Matrix::Matrix() {
}

Matrix::~Matrix() {
}

void Matrix::setColor(Rgb rgb) {
    setColor(rgb.getRed(), rgb.getGreen(), rgb.getBlue());
}

void Matrix::setColor(uint8_t r, uint8_t g, uint8_t b) {
    // use the top 5 bits
    uint16_t red = (r & 0xf0) >> 3;
    uint16_t green = (b & 0xf0) >> 3;
    uint8_t blue = (b & 0xf0) >> 3;

    color = (red << 10) | (green << 5) | blue;
}

void Matrix::setPixel(int16_t x, int16_t y) {
    x += translate_x;
    y += translate_y;

    int16_t i = (x & 0x01) ? (x * MATRIX_WIDTH + MATRIX_HEIGHT - 1 - y) : (x * MATRIX_HEIGHT + y);
    //int16_t i = (x * 12) + y;
    if (i >= MATRIX_WIDTH * MATRIX_HEIGHT || i < 0) return;

    uint16_t current = buffer[i];

    uint8_t overlay = getOverlay();
    if (overlay == DRAW_OVERLAY_REPLACE){
        buffer[i] = color;
    }
    else if (overlay == DRAW_OVERLAY_OR){
        buffer[i] |= color;
    }
    else if (overlay == DRAW_OVERLAY_NAND){
        buffer[i] &= ~color;
    }
    else if (overlay == DRAW_OVERLAY_XOR){
        buffer[i] ^= color;
    }

    if (buffer[i] != current) changed = 1;
}

void Matrix::flush(){
    if (changed) {
        ph42121rgb16s_flush(buffer);
        changed = 0;
    }
}
