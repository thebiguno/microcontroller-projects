#include "draw.h"

// Implementation of Bresenham's algorithm; adapted from Lady Ada's GLCD library,
// which was in turn adapted from Wikpedia.
void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t value, uint8_t o){

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
			set_pixel(y0, x0, value, o);
		}
		else {
			set_pixel(x0, y0, value, o);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void draw_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t f, uint8_t value, uint8_t o){
	//Make sure that x0,y0 is top left corner.
	if (x0 > x1) swap(x0, x1);
	if (y0 > y1) swap(y0, y1);

	for(int16_t x = x0; x <= x1; x++){
		for (int16_t y = y0; y <= y1; y++){
			if (f || x == x0 || x == x1 || y == y0 || y == y1) set_pixel(x, y, value, o);
		}
	}
}

void draw_bitmap(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t orientation, uint8_t* bitmap, uint8_t value, uint8_t o){
	//We need to figure out which bit the beginning of the character is, and how
	// many bytes are used for a glyph.
	uint8_t glyphByteCount = ((width * height) >> 3); //(w*h)/8, int math
	uint8_t glyphBitCount = (width * height) & 0x7; //(w*h)%8
	
	uint8_t bitCounter = 7;
	uint8_t byteCounter = 0;

	if (glyphBitCount != 0) {
		glyphByteCount++;
		bitCounter = glyphBitCount - 1; // the padding is at the front of the first byte, so don't start at bit 0
	}

	if (orientation == ORIENTATION_NORMAL){
		for(int16_t iy = y; iy < y + height; iy++){
			for(int16_t ix = x; ix < x + width; ix++){
				if (pgm_read_byte_near(bitmap + byteCounter) & _BV(bitCounter)){
					set_pixel(ix, iy, value, o);
				}
				
				if (bitCounter == 0){
					byteCounter++;
					bitCounter = 8;
				}
				
				bitCounter--;
			}
		}
	}
	else if (orientation == ORIENTATION_DOWN){
		for(int16_t ix = x + height - 1; ix >= x; ix--){
			for(int16_t iy = y; iy < y + width; iy++){
				if (pgm_read_byte_near(bitmap + byteCounter) & _BV(bitCounter)){
					set_pixel(ix, iy, value, o);
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

void draw_text(int16_t x, int16_t y, char* text, uint8_t width, uint8_t height, uint8_t orientation, uint8_t* font, uint8_t* codepage, uint8_t value, uint8_t o){
	uint8_t i = 0;
	
	//We need to figure out which bit the beginning of the character is, and how
	// many bytes are used for a glyph.
	uint8_t glyphByteCount = ((width * height) >> 3); //(w*h)/8, int math
	uint8_t glyphBitCount = (width * height) & 0x7; //(w*h)%8
	if (glyphBitCount != 0){
		glyphByteCount++;
	}
	
	while (text[i]){
		//Find the entry in the code page
		uint8_t glyphIndex = pgm_read_byte_near(codepage + (uint8_t) text[i]);

		if (glyphIndex != 0xFF){
			draw_bitmap(x, y, width, height, orientation, font + (glyphIndex * glyphByteCount), value, o);
		}

		if (orientation == ORIENTATION_NORMAL) x += (width + 1);
		else if (orientation == ORIENTATION_DOWN) y += (width + 1);
		i++;
	}
}

//Implementation of Bresenham Algorithm for a full circle, adapted from Wikipedia sample
void draw_circle(int16_t x0, int16_t y0, uint8_t r, uint8_t fill, uint8_t value, uint8_t overlay){
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;
	
	if (fill){
		draw_line(max(0, x0), max(0, y0 - r), max(0, x0), max(0, y0 + r), value, overlay);
		draw_line(max(0, x0 - r), max(0, y0), max(0, x0 + r), max(0, y0), value, overlay);
	}
	else {
		set_pixel(x0, y0 + r, value, overlay);
		set_pixel(x0, y0 - r, value, overlay);
		set_pixel(x0 + r, y0, value, overlay);
		set_pixel(x0 - r, y0, value, overlay);
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
			draw_line(max(0, x0 - x), max(0, y0 + y), max(0, x0 + x), max(0, y0 + y), value, overlay);
			draw_line(max(0, x0 - x), max(0, y0 - y), max(0, x0 + x), max(0, y0 - y), value, overlay);
			draw_line(max(0, x0 - y), max(0, y0 + x), max(0, x0 + y), max(0, y0 + x), value, overlay);
			draw_line(max(0, x0 - y), max(0, y0 - x), max(0, x0 + y), max(0, y0 - x), value, overlay);
		}
		else { 
			set_pixel(x0 + x, y0 + y, value, overlay);
			set_pixel(x0 - x, y0 + y, value, overlay);
			set_pixel(x0 + x, y0 - y, value, overlay);
			set_pixel(x0 - x, y0 - y, value, overlay);
			set_pixel(x0 + y, y0 + x, value, overlay);
			set_pixel(x0 - y, y0 + x, value, overlay);
			set_pixel(x0 + y, y0 - x, value, overlay);
			set_pixel(x0 - y, y0 - x, value, overlay);
		}
	}
}