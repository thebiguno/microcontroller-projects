#include "Draw.h"

using namespace digitalcave;

void Draw::setFont(uint8_t* font, uint8_t* codepage, uint8_t width, uint8_t height) {
	this->font = font;
	this->font_codepage = codepage;
	this->font_width = width;
	this->font_height = height;
	
	//We need to figure out which bit the beginning of the character is, and how
	// many bytes are used for a glyph.
	this->font_glyph_byte_ct = ((width * height) >> 3); //(w*h)/8, int math
	uint8_t glyph_bit_ct = (width * height) & 0x7; //(w*h)%8
	if (glyph_bit_ct != 0){
		this->font_glyph_byte_ct++;
	}
}

void Draw::setOverlay(uint8_t o) {
	overlay = o;
}
uint8_t Draw::getOverlay() {
	return overlay;
}

// Implementation of Bresenham's algorithm; adapted from Lady Ada's GLCD library,
// which was in turn adapted from Wikpedia.
void Draw::line(int16_t x0, int16_t y0, int16_t x1, int16_t y1){
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			setPixel(y0, x0);
		}
		else {
			setPixel(x0, y0);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void Draw::rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t f) {
	//Make sure that x0,y0 is top left corner.
	if (x0 > x1) swap(x0, x1);
	if (y0 > y1) swap(y0, y1);

	for(int16_t x = x0; x <= x1; x++){
		for (int16_t y = y0; y <= y1; y++){
			if (f || x == x0 || x == x1 || y == y0 || y == y1) setPixel(x, y);
		}
	}
}

void Draw::bitmap(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t orientation, uint8_t* bitmap){
	//We need to figure out which bit the beginning of the character is, and how
	// many bytes are used for a glyph.
	uint8_t byteCount = ((width * height) >> 3); //(w*h)/8, int math
	uint8_t bitCount = (width * height) & 0x7; //(w*h)%8
	
	uint8_t bitCounter = 7;
	uint8_t byteCounter = 0;

	// account for padding, if any
	if (bitCount != 0) {
		byteCount++;
		bitCounter = bitCount - 1; // the padding is at the front of the first byte, so don't start at bit 0
	}

	if (orientation == DRAW_ORIENTATION_0){
		for(int16_t iy = y; iy < y + height; iy++){
			for(int16_t ix = x; ix < x + width; ix++){
				if (pgm_read_byte_near(bitmap + byteCounter) & _BV(bitCounter)){
					setPixel(ix, iy);
				}
				if (bitCounter == 0){
					byteCounter++;
					bitCounter = 8;
				}
				bitCounter--;
			}
		}
	}
	else if (orientation == DRAW_ORIENTATION_90){
		for(int16_t ix = x + height - 1; ix >= x; ix--){
			for(int16_t iy = y; iy < y + width; iy++){
				if (pgm_read_byte_near(bitmap + byteCounter) & _BV(bitCounter)){
					setPixel(ix, iy);
				}
				if (bitCounter == 0){
					byteCounter++;
					bitCounter = 8;
				}
				bitCounter--;
			}
		}
	}
	else if (orientation == DRAW_ORIENTATION_180){
		// TODO, this isn't right
		for(int16_t iy = y + height - 1; iy >= y; iy--) {
			for(int16_t ix = x + width - 1; ix >= x; ix--) {
				if (pgm_read_byte_near(bitmap + byteCounter) & _BV(bitCounter)){
					setPixel(ix, iy);
				}
				
				if (bitCounter == 0) {
					byteCounter++;
					bitCounter = 0;
				}
				bitCounter--;
			}
		}
	}
	else if (orientation == DRAW_ORIENTATION_270){
		for (int16_t ix = x; ix < x + height; ix++){
			for (int16_t iy = y + width - 1; iy >= y; iy--) {
				if (pgm_read_byte_near(bitmap + byteCounter) & _BV(bitCounter)){
					setPixel(ix, iy);
				}
				if (bitCounter == 0){
					byteCounter++;
					bitCounter = 8;
				}
				bitCounter--;
			}
		}
	}
}

void Draw::text(int16_t x, int16_t y, const char* text, uint8_t orientation) {
	uint8_t i = 0;
	
	while (text[i]){
		//Find the entry in the code page
		uint8_t glyph_index = pgm_read_byte_near(font_codepage + (uint8_t) text[i]);

		if (glyph_index != 0xFF) {
			bitmap(x, y, font_width, font_height, orientation, font + (glyph_index * font_glyph_byte_ct));
		}

		if (orientation == DRAW_ORIENTATION_NORMAL) x += (font_width + 1);
		else if (orientation == DRAW_ORIENTATION_DOWN) y += (font_width + 1);
		i++;
	}
}

//Implementation of Bresenham Algorithm for a full circle, adapted from Wikipedia sample
void Draw::circle(int16_t x0, int16_t y0, uint8_t r, uint8_t fill){
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;
	
	if (fill){
		line(max(0, x0), max(0, y0 - r), max(0, x0), max(0, y0 + r));
		line(max(0, x0 - r), max(0, y0), max(0, x0 + r), max(0, y0));
	}
	else {
		setPixel(x0, y0 + r);
		setPixel(x0, y0 - r);
		setPixel(x0 + r, y0);
		setPixel(x0 - r, y0);
	}
	
	while(x < y) {
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - r*r + 2*x - y + 1;
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;   
		if (fill){
			line(max(0, x0 - x), max(0, y0 + y), max(0, x0 + x), max(0, y0 + y));
			line(max(0, x0 - x), max(0, y0 - y), max(0, x0 + x), max(0, y0 - y));
			line(max(0, x0 - y), max(0, y0 + x), max(0, x0 + y), max(0, y0 + x));
			line(max(0, x0 - y), max(0, y0 - x), max(0, x0 + y), max(0, y0 - x));
		}
		else { 
			setPixel(x0 + x, y0 + y);
			setPixel(x0 - x, y0 + y);
			setPixel(x0 + x, y0 - y);
			setPixel(x0 - x, y0 - y);
			setPixel(x0 + y, y0 + x);
			setPixel(x0 - y, y0 + x);
			setPixel(x0 + y, y0 - x);
			setPixel(x0 - y, y0 - x);
		}
	}
}