#include "Gif.h"

/*
 * A GIF / LZW decoder
 * Derived from an implementation by Steven A. Bennett which was in turn derived from an implementation by Steve Wilhite
 */
using namespace digitalcave;

Gif::Gif(Stream* stream) :
    stream(stream) {
}

void Gif::draw(Draw *draw, int16_t x, int16_t y, uint8_t orientation) {
    if (this->version == 0) {
        readHeader();
    }

    while (1) {
        uint8_t b;
        this->stream->read(&b);

        if (b == 0x2c) {
            this->readImage(draw, x, y, orientation);
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

    if (strncmp(this->buf, "GIF87a", 6) == 0) this->version = 87;
    else if (strncmp(this->buf, "GIF89a", 6) == 0) this->version = 89;
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
        this->stream->read(this->globalPalette, colorTableBytes);
    }
}

void Gif::readApplicationExtension() {
    uint8_t blockSize;
    while (1) {
        this->stream->read(&blockSize);             // read the block size
        this->stream->read(this->buf, blockSize);   // read the block data
        this->stream->skip(1);                      // read the block terminator

        if (strncmp(this->buf, "NETSCAPE2.0", 11) == 0
            || strncmp(this->buf, "ANIMEXTS1.0", 11) == 0) {
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
        this->useTransparentIndex = 1
        this->transparentIndex = this->buf[4];
    } else {
        this->useTransparentIndex = 0;
    }
}

void Gif::readImage(Draw *draw, int16_t offset_x, int16_t offset_y, uint8_t orientation) {
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
        this->stream->read(this->localPalette, colorTableBytes);
    }

    if (this->disposalMethod == DISPOSAL_BACKGROUND
        || this->disposalMethod == DISPOSAL_RESTORE) {
        rgb_24 rgb = this->globalPalette[index];
        draw->setRgb(rgb.red, rgb.green, rgb.blue);
        for (uint16_t i = this->x; i < this->x + this->w; i++) {
            for (uint16_t j = this->y; j < this->y + this->h; j++) {
                draw->setPixel(i, j);
            }
        }
    }

    uint8_t lzwCodeSize;    // minimum number of bits required to represent the set of pixel values
    this->stream->read(&lzwCodeSize);
    this->decode(lzwCodeSize, draw, offset_x, offset_y, orientation);
}

int16_t Gif:getNextCode() {
    int16_t result;

    if (this->bitsLeft == 0) {
        if (this->availBytes < 0) {
            // read the next block
            this->stream->read(&this->availBytes);           // read the block size
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
            this->stream->read(&this->availBytes);           // read the block size
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
}

void Gif::decode(uint8_t startSize, int16_t offset_x, int16_t offset_y, uint8_t orientation) {
    // initialize instance state required by getNextCode()
    this->codeSize = size + 1;
    this->availBytes = 0;
    this->bitsLeft = 0;
    this->codeSize = startSize;

    // wow; 12 kB required for the dictionary
    uint8_t stack[4096];                    // stack for storing pixel values
    uint8_t suffix[4096];                   // suffix table
    uint16_t prefix[4096];                  // prefix table

    uint16_t clearCode = 1 << size;         // the value for the clear code
    uint16_t stopCode = size + 1;           // the value for the stop code
    uint16_t firstCode = stopCode + 1;      // the first code value
    uint16_t topSlot = 1 << startSize;      // the highest code for the current code size
    uint16_t slot = firstCode;              // the current code value
    uint8_t *stackPtr;

    int16_t code = -1;                      // the current code read from the stream
    int16_t oldCode = -1;                   // the last code read from the stream

    uint16_t x = offset_x + this->x;
    uint16_t y = offset_y + this->y;

    while ((code = this->getNextCode()) != stopCode) {
        if (code < 0) return;

        if (code == endCode) {
            break;
        } else if (code == clearCode) {
            // re-initialize
            codeSize = size + 1;
            slot = firstCode;
            topSlot = 1 << this->codeSize;

            // deal with some unlikely conditions
            while ((code = this->getNextCode()) == clearCode) {} // multiple clear codes
            if (code == stopCode) break;    // stop code following a clear code
            if (code >= slot) code = 0;     // code is out of range

            oldCode = code;
            *stackPtr++ = code;             // push the initial root code onto the stack as a pixel index
        } else {
            // deal with some unlikely conditions
            if (code >= slot) { // code is out of range
                *stackPtr++ = oldCode;
                code = oldCode;
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
                suffix[slot] = code;
                prefix[slot++] = oldCode;
                oldCode = code;
            }
            // if the code size is exhausted, increase the code size
            if (slot >= topSlot) {
                if (this->codeSize < 12) {
                    topSlot <<= 1;
                    ++this->codeSize;
                }
            }
        }

        uint16_t max_x = offset_x + this->w;

        // pop the decoded string of indexes and draw them
        while (stackPtr > this->stack) {
            if (this->useInterlace) {
            }
            uint8_t index = *(--stackPtr);

            if (index == this->transparentIndex && this->useTransparentIndex) {
                ; // don't draw this pixel
            } else if (this->useLocalPalette) {
                rgb_24 rgb = this->localPalette[index];
                draw->setRgb(rgb.red, rgb.green, rgb.blue);
            } else {
                rgb_24 rgb = this->globalPalette[index];
                draw->setRgb(rgb.red, rgb.green, rgb.blue);
            }

            // TODO deal with interlaced images here
            // TODO deal with orientation here
            draw->setPixel(x++, y);

            if (x > max_x) {
                x = offset_x + this->x;
                y++;
            }
        }
    }
}

uint16_t Icon::getDelayMs() {
    return this->delayTime * 100;
}
