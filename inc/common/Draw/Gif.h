#ifndef GIF_H
#define GIF_H

#include <stdint.h>
#include <Draw.h>
#include <Stream.h>

#define DISPOSAL_NONE       0
#define DISPOSAL_LEAVE      1
#define DISPOSAL_BACKGROUND 2
#define DISPOSAL_RESTORE    3

typedef struct rgb_24 {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_24;

namespace digitalcave {
    class Gif {
    private:
        Stream* stream;
        Draw* draw;
        // logical display size
        uint16_t width;
        uint16_t height;
        rgb_24 globalPalette[256];

        // image descriptor
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
        rgb_24 localPalette[256];
        uint8_t useLocalPalette;
        uint8_t useInterlace;

        // control extension
        uint8_t disposalMethod;
        uint6_t delayTime;      // in 1/100 s
        uint8_t useTransparentIndex;
        uint8_t transparentIndex;

        uint8_t buf[256];       // block buffer

        // LZW stuff
        uint16_t codeSize;      // the current code size
        uint16_t availBytes;    // # bytes left in block
        uint16_t bitsLeft;      // # bits left in the current byte
        uint8_t *ptr;           // pointer to the next byte in the block
        uint8_t b;              // the current byte

        uint16_t codeMask[13] = {
            0,
            0x0001, 0x0003, 0x0007, 0x000F,
            0x001F, 0x003F, 0x007F, 0x00FF,
            0x01FF, 0x03FF, 0x07FF, 0x0FFF
        };


    public:
        Gif(Stream* stream);
        ~Gif();

        uint8_t getWidth();
        uint8_t getHeight();
        uint16_t getDelayMs();

        /* Draws the icon using onto the draw context. Call repeatedly for animated Gifs. */
        void draw(Draw *draw, int16_t x, int16_t y, uint8_t orientation);
    };
}

#endif
