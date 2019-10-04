#include "Gif.h"

/*
 * A GIF / LZW decoder
 * Derived from an implementation by Steven A. Bennett which was in turn derived from an implementation by Steve Wilhite
 */
using namespace digitalcave;

Gif::Gif(Stream* stream) :
    stream(stream) {
}

void Gif::draw(Draw *canvas, int16_t x, int16_t y, uint8_t orientation) {
    if (this->version == 0) {
        readHeader();
    }

    while (1) {
        uint8_t b;
        this->stream->read(&b);

        if (b == 0x2c) {
            this->readImage(canvas, x, y, orientation);
            return;
        } else if (b == 0x21) {
            this->stream->read(&b);
            if (b == 0xf9) {
                this->readGraphicControlExtension();
            } else if (b == 0xff) {
                this->readApplicationExtension();
            } else {
                // TODO it may be nice to support the plain text extension for scrolling text
                this->readUnknownExtension();
            }
        } else if (b == 0x3b) {
            // trailer byte encountered; EOF
            this->version = 0;
            this->stream->reset();
            return;
        }
    }
}

void Gif::readHeader() {
    this->stream->read(this->buf, 6);

    if (strncmp((char*) this->buf, "GIF87a", 6) == 0) this->version = 87;
    else if (strncmp((char*) this->buf, "GIF89a", 6) == 0) this->version = 89;
    else this->version = 0;

    // logical screen descriptor
    this->stream->read(this->buf, 7);
    this->width = (this->buf[0] << 8) | this->buf[1];
    this->height = (this->buf[2] << 8) | this->buf[3];
    uint8_t packed = this->buf[4];
    this->backgroundColorIndex = this->buf[5];
//    uint8_t par = this->buf[6];
//    if (par > 0) {
//        this->aspectRatio = (par + 15.0) / 64.0
//    } else {
//        this->aspectRatio = 0.0;
//    }

    if (packed & 0x80) {
        // global color table flag is set
        // read the global palette
        uint16_t colorCount = 1 << ((packed & 0x07) + 1);  // 2, 4, 8, 16, 32, 64, 128, 256
        uint16_t colorTableBytes = sizeof(rgb_24) * colorCount; // 6 ~ 768
        this->stream->read((uint8_t*) this->globalPalette, colorTableBytes);
    }
}

void Gif::readApplicationExtension() {
    uint8_t blockSize;
    while (1) {
        this->stream->read(&blockSize);             // read the block size
        this->stream->read(this->buf, blockSize);   // read the block data
        this->stream->skip(1);                      // read the block terminator

        if (strncmp((char*) this->buf, "NETSCAPE2.0", 11) == 0
            || strncmp((char*) this->buf, "ANIMEXTS1.0", 11) == 0) {
            this->stream->read(&blockSize);
            this->stream->read(this->buf, blockSize);

            this->loop = 1;
            this->loopCount = (this->buf[1] << 8) | this->buf[2];
        }
    }
}

void Gif::readUnknownExtension() {
    uint8_t blockSize;
    while (1) {
        this->stream->read(&blockSize);
        if (blockSize == 0) {
            break;
        } else {
            this->stream->skip(blockSize);
        }
    }
}

void Gif::readGraphicControlExtension() {
    // this extension contains no data blocks so the block terminator is read in as well
    this->stream->read(this->buf, 6);
    this->disposalMethod = (this->buf[1] >> 2) & 0x07;
    this->delayTime = (this->buf[2] << 8) | this->buf[3]; // in 1/100 s

    if (this->buf[1] & 0x01) {
        this->useTransparentIndex = 1;
        this->transparentIndex = this->buf[4];
    } else {
        this->useTransparentIndex = 0;
    }
}

void Gif::readImage(Draw *canvas, int16_t offset_x, int16_t offset_y, uint8_t orientation) {
    this->stream->read(this->buf, 8);

    this->x = (this->buf[0] << 8) | this->buf[1];
    this->y = (this->buf[2] << 8) | this->buf[3];
    this->w = (this->buf[4] << 8) | this->buf[5];
    this->h = (this->buf[6] << 8) | this->buf[7];
    uint8_t packed = this->buf[8];

    if (packed & 0x80) {
        // local color table flag is set
        // read the image palette
        uint16_t colorCount = 1 << ((packed & 0x07) + 1);  // 2, 4, 8, 16, 32, 64, 128, 256
        uint16_t colorTableBytes = sizeof(rgb_24) * colorCount; // 6 ~ 768
        this->useLocalPalette = packed >> 7;
        this->useInterlace = (packed >> 6) & 0x01;
        this->stream->read((uint8_t*) this->localPalette, colorTableBytes);
    }

    if (this->disposalMethod == DISPOSAL_BACKGROUND
        || this->disposalMethod == DISPOSAL_RESTORE) {
        rgb_24 rgb = this->globalPalette[this->backgroundColorIndex];
        canvas->setColor(rgb.red, rgb.green, rgb.blue);
        for (uint16_t i = this->x; i < this->x + this->w; i++) {
            for (uint16_t j = this->y; j < this->y + this->h; j++) {
                canvas->setPixel(i, j);
            }
        }
    }

    uint8_t lzwCodeSize;    // minimum number of bits required to represent the set of pixel values
    this->stream->read(&lzwCodeSize);
    this->decode(lzwCodeSize, canvas, offset_x, offset_y, orientation);
}

int16_t Gif::getNextCode() {
    int16_t result;
    uint8_t b;

    if (this->bitsLeft == 0) {
        if (this->availBytes < 0) {
            // read the next block
            this->stream->read(&b);                          // read the block size
            this->availBytes = b;
            this->stream->read(this->buf, this->availBytes); // read the block data
            this->stream->skip(1);                           // read the block terminator
            if (this->availBytes == 0) return -1;

            this->ptr = this->buf;
            this->b = *this->ptr++;
            this->bitsLeft = 8;
            --this->availBytes;
        }
    }

    result = this->b >> (8 - this->bitsLeft);
    while (this->codeSize > this->bitsLeft) {
        if (this->availBytes <= 0) {
            this->stream->read(&b);                          // read the block size
            this->availBytes = b;
            this->stream->read(this->buf, this->availBytes); // read the block data
            this->stream->skip(1);                           // read the block terminator
            if (this->availBytes == 0) return -1;

            this->ptr = this->buf;
            this->b = *this->ptr++;
            result |= this->b << this->bitsLeft;
            this->bitsLeft += 8;
            --this->availBytes;
        }
        this->bitsLeft -= this->codeSize;
        return result & this->codeMask[this->codeSize];
    }

    return result;
}

void Gif::decode(uint8_t startSize, Draw *canvas, int16_t offset_x, int16_t offset_y, uint8_t orientation) {
    // initialize instance state required by getNextCode()
    this->availBytes = 0;
    this->bitsLeft = 0;
    this->codeSize = startSize + 1;

    // wow; 12 kB required for the dictionary
    uint8_t stack[4096];                    // stack for storing pixel values
    uint8_t suffix[4096];                   // suffix table
    uint16_t prefix[4096];                  // prefix table

    uint16_t clearCode = 1 << startSize;    // the value for the clear code
    uint16_t stopCode = clearCode + 1;      // the value for the stop code
    uint16_t firstCode = stopCode + 1;      // the first code value
    uint16_t topSlot = 1 << this->codeSize; // the highest code for the current code size
    uint16_t slot = firstCode;              // the current code value
    uint8_t *stackPtr;

    int16_t c;                              // the current code or root read from the stream
    int16_t code = -1;                      // the current code read from the stream
    int16_t oldCode = -1;                   // the last code read from the stream
    int16_t oldRoot = -1;                   // the last root, used for patterns of the type C...C

    uint16_t x = offset_x + this->x;
    uint16_t y = offset_y + this->y;

    while ((c = this->getNextCode()) != stopCode) {
        if (c < 0) return;

        if (c == stopCode) {
            break;
        } else if (c == clearCode) {
            // re-initialize
            this->codeSize = startSize + 1;
            slot = firstCode;
            topSlot = 1 << this->codeSize;

            // deal with some unlikely conditions
            while ((code = this->getNextCode()) == clearCode) {} // multiple clear codes
            if (code == stopCode) break;    // stop code following a clear code
            if (code >= slot) code = 0;     // code is out of range

            oldCode = oldRoot = code;
            *stackPtr++ = code;             // push the initial root code onto the stack as a pixel index
        } else {
            code = c;                       // the value is a

            // code is out of range
            // it's normal for code == slot (i.e. using the code about to be defined)
            // this happens for patterns of the type C....C
            // it's not normal for code > slot but deal with it in the same way anyways
            //
            if (code >= slot) {
                code = oldCode;
                *stackPtr++ = oldRoot;
            }

            // while the code is non-root, push the suffix of the code
            // then finish up by pushing the final root code
            while (code >= firstCode) {
                *stackPtr++ = suffix[code];
                code = prefix[code];
            }
            *stackPtr++ = code;


            // if there is still room for a new code at this code size, set up the prefix and suffix
            if (slot < topSlot) {
                suffix[slot] = oldRoot = code;
                prefix[slot++] = oldCode;
                oldCode = c; // what was originally read
            }

            // if the code size is exhausted, increase the code size for the next read
            if (slot >= topSlot) {
                if (this->codeSize < 12) {
                    topSlot <<= 1;
                    ++this->codeSize;
                }
            }
        }

        uint16_t max_x = offset_x + this->w;

        // pop the decoded string of indexes and draw them
        while (stackPtr > stack) {
            if (this->useInterlace) {
            }
            uint8_t index = *(--stackPtr);

            if (index == this->transparentIndex && this->useTransparentIndex) {
                ; // don't draw this pixel
            } else if (this->useLocalPalette) {
                rgb_24 rgb = this->localPalette[index];
                canvas->setColor(rgb.red, rgb.green, rgb.blue);
            } else {
                rgb_24 rgb = this->globalPalette[index];
                canvas->setColor(rgb.red, rgb.green, rgb.blue);
            }

            // TODO deal with interlaced images here
            // TODO deal with orientation here
            canvas->setPixel(x++, y);

            if (x > max_x) {
                x = offset_x + this->x;
                y++;
            }
        }
    }
}

uint16_t Gif::getDelayMs() {
    return this->delayTime * 100;
}


/*
Here is a worked example of an 8x8 B&W image -- a white field with black dots where the odd rows intersect the odd columns

00000000: 4749 4638 3761 0800 0800 8001 0000 0000  GIF87a..........
4749 4638: GIF87a (signature / version)
0800: logical width 8
0800: logical height 8
80: packed global color table set, color table size 0 (1 bit / 2 colors / 6 bytes)
01: background color index 0
00: PAR 0
00 0000: RGB0 black

00000010: ffff ff2c 0000 0000 0800 0800 0002 0c44  ...,...........D
ffff ff = RGB1 white
2c = image separator
0000: left position 0
0000: right position 0
0800: image width 8
0800: image height 8
00: packed
02: LZW starting code size (2 bits per root, 4 roots)
		clear code = 4; stop code = 5;
		top slot = 8; first code = 6; slot = 6
		2+2 three bit codes, 8 four bit codes, 16 five bit codes, etc…
0c: block size 12 bytes
44: 100 (4 clear) 000 (0 B) _01

00000020: 8ca1 bb99 ccdc 8171 3e1a 0a00 3b         .......q>...;
8c: 0__ (1 W; 6=0|1 B|W) 110 (6 BW; 7=1|0 W|B) 1000 (8! BW_B; 8=6|0 BW|B)
a1: 0001 (1 W; 9=8|1 BWB|W) EOR 1010 (10! W_W; 10=7|1 W|W)
bb: 1011 (11! WW_W; 11=10|1 WW|W) 1011 (11 WWW; 12=11|1 WWW|W) EOR
99: 1001 (9 BWBW; 13=12|0 WWWW|B) 1001 (9 BWBW; 14=9|0 BWBWB) EOR
cc: 1100 (12 WWWW; 15=9|1 BWBW|W) _1100
dc: 0____ (12 WWWW; 16=12|1 WWWW|W) EOR 01110 (14 BWBWB; 17=12|0 WWWW|B) ___11
81: 001__ (7 WB; 18=14|1 BWBWB|W) 10000 ~EOR (16 WWWWW; 19=7|1 WB|W)
71: 10001 (17 WWWWB; 20=16|W WWWWW|W) ~EOR __011
3e: 10___ (19 WBW; 21=17|1 WWWWBW) 01111 ~EOR (15 BWBWW; 22=19|1 WBWW) ____0
1a: 1010_ (20 WWWWWW; 23=15|1 BWBWW|W) _0001
0a: 0____ (1 W) 00101 (5 stop) 00 (unused)
00: 0000 (unused)
3b: terminator



And here is a partially worked example of a 32x32 RGBW repeating pattern

00000000: 4749 4638 3761 2000 2000 a103 0000 00ff  GIF87a . .......
standard header
00 00ff: RGB0 blue
00000010: ff00 0000 ff00 ffff ff2c 0000 0000 2000  .........,.... .
ff00 00: RGB1 red
00 ff00: RGB2 green
ffff ff: RGB3 white
2c 0000 0000 2000: separator, x, y, width
00000020: 2000 0002 438c 3086 9ad7 eb98 8c14 da87   ...C.0.........
2000: height
00: packed
02: LZW starting code size (2 bits per root, 4 roots)
		clear code = 4; stop code = 5;
		top slot = 8; first code = 6; slot = 6
43: block size = 67
8c: 100 (4 clear) 001 (1 R) _10
30: 0__ (2 G; 6=1|2 R|G) 000 (0 B; 7=2|0 G|B) 0011 (3 W; 8=0|3 B|W)
86: 0110 (6 RG; 9=3|2 W|R) 1000 (8 BW; 10=6|0 RG|B)
9a: 1010 (10 RGB; 11=8|3 BW|R) 1001 (9 WR; 12=10|3 RGB|W)
d7: 0111 (7 GB; 13=9|2 WR|G) 1101 (13 WRG; 14=7|3 GB|W)
eb: 1011 (11 BWR; 15=13|0 WRG|B) _1110
98: 0____ (14 GBW; 16=11|2 BWR|G) 01100 (12 RGBW; 17=14|1 GBW|R) ___10
8c: __100 (18! RGBW_R; 18=12|1 RGBW|R) EOR 10001 (17 GBWR; 19=18|2 RGBWR|G)
14: 10100 (20! GBWR_G; 20=17|2 GBWR|G) __000
da: 10___ (16 BWRG; 21=20|0 GBWRG|B) 10110 (22! BWRG_B; 22=16|0 BWRG|B) ____1
87: 0111_ (15 WRGB; 23=22|3 BWRGB|W) _1000

00000030: e7d5 b9f6 ed71 6248 82e6 878e a79a 96ed  .....qbH........
00000040: eab2 724c c3f6 8bcf b79e d7fd eee3 0983  ..rL............
00000050: 44a0 f187 1c1e 95c9 6273 e964 4aa3 54a8  D.......bs.dJ.T.
00000060: f589 9d5e b559 5701 003b                 ...^.YW..;

*/
