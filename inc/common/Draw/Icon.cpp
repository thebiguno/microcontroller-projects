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
		case 1: bpp = 4; bytes = (width * height) >> 1; bits = ((width * height) & 0x1) << 2; break;
		case 2: bpp = 4; bytes = (width * height) >> 1; bits = ((width * height) & 0x1) << 2; break;
		case 3: bpp = 8; bytes = width * height; bits = 0; break;
		case 4: bpp = 8; bytes = width * height; bits = 0; break;
		case 5: bpp = 12; bytes = (width * height) + ((width * height) >> 1); bits = ((width * height) & 0x1) << 2; break;
		case 6: bpp = 16; bytes = (width * height) << 1; bits = 0; break;
		case 7: bpp = 24; bytes = (width * height) * 3; bits = 0; break;
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
		case 0: this->draw0(draw, x, y bit, pixel); break;
		case 1: this->draw1(draw, bit, pixel); break;
		case 2: this->draw2(draw, bit, pixel); break;
		case 3: this->draw3(draw, pixel); break;
		case 4: this->draw4(draw, pixel); break;
		case 5: this->draw5(draw, pixel); break;
		case 6: this->draw6(draw, pixel); break;
		case 7: this->draw7(draw, pixel); break;
	}
	if (palette > 0) {
		draw->setPixel(x,y);
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
void Icon::draw1(Draw* draw, uint8_t bit, uint8_t* pixel) {
	uint8_t l3 = (pixel[0] & bv(bit--)) ? 8 : 0;
	uint8_t l2 = (pixel[0] & bv(bit--)) ? 4 : 0;
	uint8_t l1 = (pixel[0] & bv(bit--)) ? 2 : 0;
	uint8_t l0 = (pixel[0] & bv(bit--)) ? 1 : 0;
	uint8_t l = (l3 | l2 | l1 | l0) * 0x11;
	if (l > 0) {
		draw->setColor(l,l,l);
	}
}

// 2: 4bpp RGBI = 1bit high/low, 1bit red, 1bit green, 1bit blue (16 colours, no alpha)
// L = 0x22, 0xaa
// H = 0x77, 0xff
// simulates CGA / Tandy palette
// this mode is irregular and requires a minimum of 6 bpp in a regular RGB framebuffer
void Icon::draw2(Draw* draw, uint8_t bit, uint8_t* pixel) {
	uint8_t i = (pixel[0] & bv(bit--));
	uint8_t r = (pixel[0] & bv(bit--));
	uint8_t g = (pixel[0] & bv(bit--));
	uint8_t b = (pixel[0] & bv(bit--));

	r*=0x8
	g*=0x8
	b*=0x8
	i*=0x5

	if (r & g & !b & !i) {
		// use an orange/brown instead of a dark yellow (just like CGA)
		draw->setColor(0xaa, 0x55, 0x22);
	} else {
		draw->setColor(r+i+2, g+i+2, b+i+2);
	}
}

// 3: 8bpp RRGGBBII (256 colours)
// simulates Sam Coupé 7-bit palette but with 2 bits for intensity
// L0 = 0x00, 0x44, 0x88, 0xcc
// L1 = 0x11, 0x55, 0x99, 0xdd
// L2 = 0x22, 0x66, 0xaa, 0xee
// L3 = 0x33, 0x77, 0xbb, 0xff
// this mode is irregular and requires a minimum of 8 bpp in a regular RGB framebuffer
void Icon::draw3(Draw* draw, uint8_t* pixel) {
	uint8_t i = ((pixel[0] >> 6) & 0x3) * 0x11;
	uint8_t r = ((pixel[0] >> 4) & 0x3) * 0x44;
	uint8_t g = ((pixel[0] >> 2) & 0x3) * 0x44;
	uint8_t b = ((pixel[0] >> 0) & 0x3) * 0x44;

	draw->setColor(r+i, g+i, b+i);
}

// 4: 8bpp RRRGGGBB (256 colours)
void Icon::draw4(Draw* draw, uint8_t* pixel) {
	// r & g have the values 0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff
	// b has the values            0x33,       0x77,       0xbb,       0xff
	uint8_t r = pixel[0] >> 5;
	r = (r * 0x22) + 0x11;
	uint8_t g = (pixel[0] >> 2) & 0x07;
	g = (g * 0x22) + 0x11;
	uint8_t b = pixel[0] & 0x03;
	b = (b * 0x44) + 0x33;
	draw->setColor(r,g,b);
}

// 5: 12bpp RRRRGGGGBBBB (4096 colours)
// this is like using web colors like #rgb instead of #rrggbb
void Icon::draw5(Draw* draw, uint8_t bit, uint8_t* pixel) {
	uint8_t i = 0;
	uint8_t r = (pixel[i] >> (bit == 7 ? 0 : 4)) & 0xf
	r = (r << 4) | r
	bit-=4
	if (bit > 7) {
		bit = 7; i++;
	}
	uint8_t g = (pixel[i] >> (bit == 7 ? 0 : 4)) & 0xf
	g = (g << 4) | g
	bit-=4
	if (bit > 7) {
		bit = 7; i++;
	}
	uint8_t b = (pixel[i] >> (bit == 7 ? 0 : 4)) & 0xf
	b = (g << 4) | b
	draw->setColor(r,g,b)
}

// 6: 16bpp RRRRRGGG GGGBBBBB (65,535 colours)
void Icon::draw6(Draw* draw, uint8_t* pixel) {
	// g have the values 0x00, 0x04, 0x08, 0x12, 0x16, 0x20, ..., 0xfb, 0xff
	// r & b has the values 0x00, 0x08, 0x10, 0x19, 0x21, ..., 0xf7, 0xff

	// TODO should this be re-worked to preseve grays?
	uint8_t r = (pixel[0] >> 3);
	uint8_t g = ((pixel[0] & 0x7) << 3) | (pixel[1] >> 5);
	uint8_t b = (pixel[1] & 0x1f);

	R8 = (r * 527 + 23) >> 6;
	G8 = (g * 259 + 33) >> 6;
	B8 = (b * 527 + 23) >> 6;

	draw->setColor(r,g,b);
}

// 7: 24bpp RRRRRRRR GGGGGGGG BBBBBBBB
void Icon::draw7(Draw* draw, uint8_t* pixel) {
	draw->setColor(pixel[0], pixel[1], pixel[2], 0xff);
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
