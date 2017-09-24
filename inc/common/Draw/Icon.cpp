#include "Icon.h"

using namespace digitalcave;

Icon::Icon(Stream* stream) :
	stream(stream),
	footer(0x40) // has more frames, no delay, no loop
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
	if (config & 0x80) {
		bytes++; // footer byte
	}

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
	return (this->footer & 0x40) ? 1 : 0;
}

uint8_t Icon::hasLoop() {
	return (this->footer & 0x80) ? 1 : 0;
}

uint8_t Icon::getDelay() {
	return this->footer && 0x3F;
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

//0: 1bpp = 1bit monochrome (off, on)
// no rgb value, drawn in active draw context colour
void Icon::draw0(Draw* draw, int16_t x, int16_t y, uint8_t bit, uint8_t* pixel) {
	if (pixel[0] & bv(bit--)){
		draw->setPixel(x, y);
	}
}

// 1: 4bpp = 4bit monochrome, (16 gray levels)
// rgb hex values 00, 11, 22, ... FF
void Icon::draw1(Draw* draw, int16_t x, int16_t y, uint8_t bit, uint8_t* pixel) {
	uint8_t l3 = (pixel[0] & bv(bit--)) ? 8 : 0;
	uint8_t l2 = (pixel[0] & bv(bit--)) ? 4 : 0;
	uint8_t l1 = (pixel[0] & bv(bit--)) ? 2 : 0;
	uint8_t l0 = (pixel[0] & bv(bit--)) ? 1 : 0;
	uint8_t l = (l3 | l2 | l1 | l0) * 0x11;
	if (l > 0) {
		draw->setColor(l,l,l,0xff);
		draw->setPixel(x, y);
	}
}

// 2: 4bpp RGBI = 1bit high/low, 1bit red, 1bit green, 1bit blue (16 colours, no alpha)
// rgb hex values 00, 55, AA, FF
// L = 00, AA
// H = 55, FF
// this mode is just like CGA
// this mode is irregular and requires a minimum of 6 bpp in a regular RGB framebuffer
void Icon::draw2(Draw* draw, int16_t x, int16_t y, uint8_t bit, uint8_t* pixel) {
	uint8_t r = (pixel[0] & bv(bit--)) * 0x55;
	uint8_t g = (pixel[0] & bv(bit--)) * 0x55;
	uint8_t b = (pixel[0] & bv(bit--)) * 0x55;
	uint8_t i = (pixel[0] & bv(bit--)) * 0x55;
	if (r & g & !b & !i) {
		// use an orange/brown instead of a dark yellow (just like CGA)
		draw->setColor(0xaa, 0x55, 0x00);
	} else {
		draw->setColor(r+i, g+i, b+i, 0xff);
	}
	draw->setPixel(x, y);
}

// 3: 8bpp R2G2B2I1A1 (128 colours, on/off alpha)
// L = 11, 55, 99, DD
// H = 33, 77, BB, FF
void Icon::draw3(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t a = (pixel[0] & 0x1) * 0xff;
	uint8_t i = ((pixel[0] >> 1) & 0x1) << 2;
	uint8_t r = pixel[0] >> 6;
	r = ((r | i) << 1) | 0x1;		// add intensity, add 1
	r = (r << 4) | r;
	uint8_t g = (pixel[0] >> 4) & 0x3;
	g = ((g | i) << 1) | 0x1;
	g = (g << 4) | g;
	uint8_t b = (pixel[0] >> 4) & 0x3;
	b = ((b | i) << 1) | 0x1;
	b = (b << 4) | b;
	draw->setColor(r, g, b, a);
	draw->setPixel(x, y);
}

// 4: 16bpp R4G4B4A4 (4,096 colours)
// this mode requires 12 bpp in the framebuffer
// rgb hex values 00, 11, 22, ... FF
void Icon::draw4(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t r = (pixel[0] >> 4);
	r = (r << 4) | r;
	uint8_t g = (pixel[0] & 0xf);
	g = (g << 4) | g;
	uint8_t b = (pixel[1] >> 4);
	b = (b << 4) | b;
	uint8_t a = (pixel[1] & 0xf);
	a = (a << 4) | a;
	draw->setColor(r,g,b,a);
	draw->setPixel(x, y);
}

// 5: 16bpp R5G5B5A1 (32,767 colours)
// this mode requires a minimum of 15 bpp in the framebuffer
void Icon::draw5(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t r = (pixel[0] >> 3);
	r = (r << 3) | (r >> 2);
	uint8_t g = ((pixel[0] & 0x7) << 2) | (pixel[1] >> 6);
	g = (g << 3) | (g >> 2);
	uint8_t b = ((pixel[1] & 0x3e) >> 1);
	b = (b << 3) | (b >> 2);
	uint8_t a = (pixel[1] & 0x1) * 0xff;
	draw->setColor(r,g,b,a);
	draw->setPixel(x, y);
}

// 6: 16bpp R5G6B5 (65,535 colours)
// this mode requires a minimum of 18 bpp in the framebuffer
void Icon::draw6(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	uint8_t r = (pixel[0] >> 3);
	r = (r << 3) | (r >> 2);
	uint8_t g = ((pixel[0] & 0x7) << 3) | (pixel[1] >> 5);
	g = (g << 2) | (g >> 3);
	uint8_t b = (pixel[1] & 0x1f);
	b = (b << 3) | (b >> 2);
	draw->setColor(r,g,b,0xff);
	draw->setPixel(x, y);
}

// 7: 24bpp R8G8B8
// this mode requires a minimum of 24 bpp in the framebuffer
void Icon::draw7(Draw* draw, int16_t x, int16_t y, uint8_t* pixel) {
	draw->setColor(pixel[0], pixel[1], pixel[2], 0xff);
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

	if (this->config & 0x80) {
		this->footer = buf[i];
	}
	free(buf);
}

inline uint8_t Icon::bv(uint8_t bit) {
	return (1 << (bit));
}
