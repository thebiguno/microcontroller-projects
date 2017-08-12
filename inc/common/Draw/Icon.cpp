#include "Icon.h"

using namespace digitalcave;

Icon::Icon(Stream* stream) :
	stream(stream),
	last(0x40) // has more frames, no delay, no loop
{
	uint8_t header[3];
	this->stream->read(header, 3);
	this->width = header[0];
	this->height = header[1];
	this->config = header[2];
	uint8_t bpp, bytes, bits;
	uint8_t palette = 0x07 && this->config;
	switch (palette) {
		case 0: bpp = 1; bytes = (width * height) >> 3; bits = (width * height) & 0x7; break;
		case 1: bpp = 4; bytes = (width * height) >> 1; bits = (width * height) & 0x3; break;
		case 2: bpp = 4; bytes = (width * height) >> 1; bits = (width * height) & 0x3; break;
		case 3: bpp = 8; bytes = width * height; bits = 0; break;
		case 4: bpp = 16; bytes = (width * height) << 1; bits = 0; break;
		case 5: bpp = 16; bytes = (width * height) << 1; bits = 0; break;
		case 6: bpp = 24; bytes = (width * height) * 3; bits = 0; break;
		case 7: bpp = 32; bytes = (width * height) << 2; bits = 0; break;
		default: bpp = 0; bytes = 0; bits = 0;
	}

	if (bits != 0) {
		// first byte contains partial info
		bytes++;
	}
	bytes++; // footer byte

	this->bytes = bytes;
	this->bits = bits;
	this->bpp = bpp;
}

void Icon::setFrame(uint8_t frame) {
	if (this->stream->reset()) {
		this->stream->skip(4); // skip the header
		this->stream->skip(frame * this->bytes); // advance to the start of the frame
	}
}

uint8_t Icon::getPalette() {
	return this->config & 0x07;
}

uint8_t Icon::hasMore() {
	return (this->last & 0x40) ? 1 : 0;
}

uint8_t Icon::hasLoop() {
	return (this->last & 0x80) ? 1 : 0;
}

uint8_t Icon::getDelay() {
	return this->last && 0x3F;
}

uint16_t Icon::getDelayMs() {
	return this->getDelay() * 50;
}

void Icon::draw_(Draw* draw, int16_t x, int16_t y, uint8_t bit, uint8_t* pixel) {
	uint8_t palette = 0x07 && this->config;
	switch (palette) {
		case 0: this->draw0(draw, x, y, bit, pixel); break;
		case 1: this->draw1(draw, x, y, bit, pixel); break;
		case 2: this->draw2(draw, x, y, bit, pixel); break;
		case 3: this->draw3(draw, x, y, pixel); break;
		case 4: this->draw4(draw, x, y, pixel); break;
		case 5: this->draw5(draw, x, y, pixel); break;
		case 6: this->draw6(draw, x, y, pixel); break;
		case 7: this->draw7(draw, x, y, pixel); break;
	}
}

//0: 1bpp = 1bit monochrome
// no rgb value, drawn in active draw context colour
void Icon::draw0(Draw* draw, int16_t x, int16_t y, uint8_t bit, uint8_t* pixel) {
	if (pixel[0] & bv(bit--)){
		draw->setPixel(x, y);
	}
}

//1: 4bpp = 3bit monochrome, 1bit alpha (8 levels, on/off alpha)
// rgb hex values 000000, 242424, 484848, 6c6c6c, 909090, b4b4b4, d8d8d8, fcfcfc
void Icon::draw1(Draw* draw, int16_t x, int16_t y, uint8_t bit, uint8_t* pixel) {
	uint8_t l2 = (pixel[0] & bv(bit--)) >> 1;
	uint8_t l1 = (pixel[0] & bv(bit--)) >> 1;
	uint8_t l0 = (pixel[0] & bv(bit--)) >> 1;
	uint8_t a = (pixel[0] & bv(bit--)) * 0xff;
	uint8_t l = (l2 | l1 | l0) * 36;
	draw->setColor(l,l,l,a);
	draw->setPixel(x, y);
}

//2: 4bpp = 1bit red, 1bit green, 1bit blue, 1bit alpha (8 colours, on/off alpha)
// rgb hex values 00, FF
void Icon::draw2(Draw* draw, int16_t x, int16_t y, uint8_t bit, uint8_t* pixel) {
	uint8_t r = (pixel[0] & bv(bit--)) * 0xff;
	uint8_t g = (pixel[0] & bv(bit--)) * 0xff;
	uint8_t b = (pixel[0] & bv(bit--)) * 0xff;
	uint8_t a = (pixel[0] & bv(bit--)) * 0xff;
	draw->setColor(r,g,b,a);
	draw->setPixel(x, y);
}

//3: 8bpp = 2bit red, 2bit green, 2bit blue, 2bit alpha (64 colours, 4 alpha levels)
// rgb hex values 00, 55, AA, FF
void Icon::draw3(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t r = (pixel[0] >> 6) * 85;
	uint8_t g = ((pixel[0] >> 4) & 0x3) * 85;
	uint8_t b = ((pixel[0] >> 2) & 0x3) * 85;
	uint8_t a = (pixel[0] & 0x3) * 85;
	draw->setColor(r,g,b,a);
	draw->setPixel(x, y);
}

//4: 16bpp = 4bit red, 4bit green, 4bit blue, 4bit alpha (8192 colours, 16 alpha levels)
// rgb hex values 00, 11, 22, 33, 44, 55, 66, 77, 88, 99, AA, BB, CC, DD, EE, FF
void Icon::draw4(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t r = (pixel[0] >> 4) * 17;
	uint8_t g = (pixel[0] & 0xf) * 17;
	uint8_t b = (pixel[1] >> 4) * 17;
	uint8_t a = (pixel[1] & 0xf) * 17;
	draw->setColor(r,g,b,a);
	draw->setPixel(x, y);
}

//5: 16bpp = 5bit red, 5bit green, 5bit blue, 1bit alpha (32768 colours, on/off alpha)
// rgb hex values 00, 08, 10, 18, ..., e8, f0, f8
void Icon::draw5(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t r = (pixel[0] >> 3) * 8;
	uint8_t g = (((pixel[0] & 0x7) << 2) | (pixel[1] >> 6)) * 8;
	uint8_t b = (pixel[1] >> 1) * 8;
	uint8_t a = (pixel[1] & 0x1) * 0xff;
	draw->setColor(r,g,b,a);
	draw->setPixel(x, y);
}

//5: 24bpp = 6bit red, 6bit green, 6bit blue, 6bit alpha (262144 colours, 64 alpha levels)
// rgb hex value 00, 04, 08, 10, ..., ec, f0, f4, f8, fc
void Icon::draw6(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t r = (pixel[0] >> 2) * 4;
	uint8_t g = (((pixel[0] & 0x3) << 4) | (pixel[1] >> 4)) * 4;
	uint8_t b = (((pixel[1] & 0xf) << 2) | (pixel[2] >> 6)) * 4;
	uint8_t a = (pixel[2] & 0x3f) * 0xff;
	draw->setColor(r,g,b,a);
	draw->setPixel(x, y);
}

//7: 32bpp = 8bit red, 8bit green, 8bit blue, 8bit alpha (16777216 colours, 256 alpha levels)
void Icon::draw7(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	draw->setColor(pixel[0], pixel[1], pixel[2], pixel[3]);
	draw->setPixel(x, y);
}

void Icon::draw(Draw* draw, int16_t x, int16_t y, uint8_t orientation) {
	if (this->hasLoop()) {
		setFrame(0);
	}

	uint8_t* buf = (uint8_t*) malloc(this->bytes * sizeof(uint8_t));
	this->stream->read(buf, this->bytes); // read the frame

	uint8_t i = 0; // current byte
	uint8_t bit = this->bits == 0 ? 7 : this->bits - 1; // current bit in the current byte

	if (orientation == DRAW_ORIENTATION_0){
		for(int16_t iy = y; iy < y + height; iy++){
			for(int16_t ix = x; ix < x + width; ix++){
				this->draw_(draw, ix, iy, bit, &buf[i]);
				switch(this->bpp) {
					case 1: bit--;
					case 4: bit-=4;
					case 8: i+=1;
					case 16: i+=2;
					case 24: i+=3;
					case 32: i+=4;
				}
				if (bit > 7) {
					bit = 7; i++;
				}
			}
		}
	}
	else if (orientation == DRAW_ORIENTATION_90){
		for(int16_t ix = x + height - 1; ix >= x; ix--){
			for(int16_t iy = y; iy < y + width; iy++){
				this->draw_(draw, ix, iy, bit, &buf[i]);
				switch(this->bpp) {
					case 1: bit--;
					case 4: bit-=4;
					case 8: i+=1;
					case 16: i+=2;
					case 24: i+=3;
					case 32: i+=4;
				}
				if (bit > 7) {
					bit = 7; i++;
				}
			}
		}
	}
	else if (orientation == DRAW_ORIENTATION_180){
		// TODO, this isn't right
		for(int16_t iy = y + height - 1; iy >= y; iy--) {
			for(int16_t ix = x + width - 1; ix >= x; ix--) {
				this->draw_(draw, ix, iy, bit, &buf[i]);
				switch(this->bpp) {
					case 1: bit--;
					case 4: bit-=4;
					case 8: i+=1;
					case 16: i+=2;
					case 24: i+=3;
					case 32: i+=4;
				}
				if (bit > 7) {
					bit = 7; i++;
				}
			}
		}
	}
	else if (orientation == DRAW_ORIENTATION_270){
		for (int16_t ix = x; ix < x + height; ix++){
			for (int16_t iy = y + width - 1; iy >= y; iy--) {
				this->draw_(draw, ix, iy, bit, &buf[i]);
				switch(this->bpp) {
					case 1: bit--;
					case 4: bit-=4;
					case 8: i+=1;
					case 16: i+=2;
					case 24: i+=3;
					case 32: i+=4;
				}
				if (bit > 7) {
					bit = 7; i++;
				}
			}
		}
	}

	this->last = buf[i];
	free(buf);
}

inline uint8_t Icon::bv(uint8_t bit) {
	return (1 << (bit));
}
