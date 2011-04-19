#include "draw.h"

// Implementation of Bresenham's algorithm; adapted from Lady Ada's GLCD library,
// which was in turn adapted from Wikpedia.
void glcd_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t o){

	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	uint8_t dx, dy;
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

	for (; x0 < x1; x0++) {
		if (steep) {
			glcd_set_pixel(y0, x0, o);
		}
		else {
			glcd_set_pixel(x0, y0, o);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void glcd_draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t f, uint8_t o){
	//Make sure that x0,y0 is top left corner.
	if (x0 > x1) swap(x0, x1);
	if (y0 > y1) swap(y0, y1);

	for(uint8_t x = x0; x <= x1; x++){
		for (uint8_t y = y0; y <= y1; y++){
			if (f || x == x0 || x == x1 || y == y0 || y == y1) glcd_set_pixel(x, y, o);
		}
	}
}

void glcd_draw_text(uint8_t x, uint8_t y, char* text, uint8_t width, uint8_t height, prog_uchar* font, prog_uchar* codepage, uint8_t o){
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

		uint8_t bitCounter = glyphBitCount - 1;
		uint8_t byteCounter = 0;
		
		for(uint8_t iy = y; iy < y + height; iy++){
			for(uint8_t ix = x; ix < x + width; ix++){
				if (pgm_read_byte_near(font + (glyphIndex * glyphByteCount) + byteCounter) & _BV(bitCounter)){
					glcd_set_pixel(ix, iy, o);
				}
				
				if (bitCounter == 0){
					byteCounter++;
					bitCounter = 8;
				}
				
				bitCounter--;
			}
		}

		x += (width + 1);
		i++;
	}
}
