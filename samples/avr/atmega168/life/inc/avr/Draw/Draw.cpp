#include "../../common/Draw/Draw.h"

using namespace digitalcave;

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
