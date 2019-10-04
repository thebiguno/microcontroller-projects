#include "Buffer.h"

using namespace digitalcave;

Buffer::Buffer(uint8_t width, uint8_t height) :
	width(width),
	height(height)
{
	data = (uint8_t*) malloc((width * height) >> 3);
	clear();
}

uint8_t* Buffer::get_data(){
	return data;
}

void Buffer::clear(){
	for (uint8_t x = 0; x < (width >> 3); x++){
		for (uint8_t y = 0; y < height; y++){
			data[(y * (width >> 3)) + x] = 0x00;
		}
	}
}

void Buffer::set_pixel(int16_t x, int16_t y, uint8_t value){
	if ((x >= 0 && x < width) && (y >= 0 && y < height)){
		if (value){
			data[(y * (width >> 3)) + (x >> 3)] |= _BV(0x07 - (x & 0x07));
		}
		else {
			data[(y * (width >> 3)) + (x >> 3)] &= ~_BV(0x07 - (x & 0x07));
		}
	}
}

int16_t Buffer::write_char(char c, font_t font, int16_t x, int16_t y){
	uint8_t width = font.width;
	uint8_t height = font.height;

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

	uint8_t glyph_index = pgm_read_byte_near(font.codepage + (uint8_t) c);
	if (glyph_index != 0xFF){
		for (int16_t iy = y; iy < y + height; iy++){
			for (int16_t ix = x; ix < x + width; ix++){
				if (pgm_read_byte_near(font.font_data + (glyph_index * byteCount) + byteCounter) & _BV(bitCounter)){
					set_pixel(ix, iy, 1);
				}
				if (bitCounter == 0){
					byteCounter++;
					bitCounter = 8;
				}
				bitCounter--;
			}
		}
	}

	uint8_t char_width = font.variable_width == FONT_VARIABLE_WIDTH ? pgm_read_byte_near(font.font_widths + glyph_index) : 0xFF;
	return char_width == 0xFF ? font.width : char_width;
}

int16_t Buffer::write_string(const char* text, font_t font, int16_t x, int16_t y){
	return write_string((char*) text, font, x, y);
}
int16_t Buffer::write_string(char* text, font_t font, int16_t x, int16_t y){
	uint8_t i = 0;
	uint8_t result = 0;

	while (text[i]) {
		uint8_t width = write_char(text[i], font, x, y);
		result += width + 1;
		i++;
		x += width + 1;	//Font width + 1
	}

	return result;
}
