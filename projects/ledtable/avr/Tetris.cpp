#include <Tetris.h>
#include <Matrix.h>
#include <lib/Psx/Psx.h>
#include <lib/Draw/Hsv.h>
#include <lib/Draw/Rgb.h>

using namespace digitalcave;

	// each piece is a 4x4 bitmap encoded in 16-bits and there are 4 rotations for each piece
	uint8_t pieces[56] PROGMEM = {
		0x06,0x60, 0x06,0x60, 0x06,0x60, 0x06,0x60, // O
		0x0f,0x00, 0x22,0x22, 0x0f,0x00, 0x22,0x22, // I
		0x03,0x60, 0x23,0x10, 0x03,0x60, 0x23,0x10, // S
		0x06,0x30, 0x13,0x20, 0x06,0x30, 0x13,0x20, // Z
		0x07,0x40, 0x22,0x30, 0x17,0x00, 0x62,0x20, // L
		0x07,0x10, 0x32,0x20, 0x47,0x00, 0x22,0x60, // J
		0x07,0x20, 0x23,0x20, 0x27,0x00, 0x26,0x20 // T
	};
	
	// each piece bitmap is painted in has a specific color
	uint16_t hues[7] = {
		// yellow, cyan, green, red, orange, blue, violet
		60, 180, 120, 0, 30, 240, 270
	};
	
extern Matrix matrix;
extern Psx psx;
extern Hsv hsv;

Tetris::Tetris() {
}

Tetris::~Tetris() {
}

void Tetris::run() {
	uint8_t running = 1;
	newGame();
	
	while (running) {
		uint16_t buttons;
		uint16_t changed;
		
		generate();
		
		psx.poll();
		buttons = psx.buttons();
		changed = psx.changed();
		
		if (buttons & PSB_PAD_UP && changed & PSB_PAD_UP) rotate();
		else if (buttons & PSB_PAD_DOWN && changed & PSB_PAD_DOWN) down();
		else if (buttons & PSB_PAD_LEFT && changed & PSB_PAD_LEFT) left();
		else if (buttons & PSB_PAD_RIGHT && changed & PSB_PAD_RIGHT) right();
		else if (buttons & PSB_TRIANGLE && changed & PSB_TRIANGLE) running = 0;
		
		
	}
}

void Tetris::flush() {
	Hsv h = Hsv(hsv);
	for (uint8_t x = 0; x < 12; x++) {
		for (uint8_t y = 0; y < 12; y++) {
			uint8_t s = state[x][y];
			if (s < 7) {
				h.setHue(hues[s]);
				matrix.setColor(Rgb(h));
			} else {
				matrix.setColor(0,0,0);
			}
			matrix.setPixel(x,y);
		}
	}
	matrix.flush();
}

uint8_t Tetris::newGame() {
	for (uint8_t x = 0; x < 12; x++) {
		for (uint8_t y = 0; y < 12; y++) {
			state[x][y] = 255;
		}
	}
	flush();
	return 0;
}

void Tetris::generate() {
	orientation = 0;
	uint8_t r = rand() % 7;
	uint8_t offset = 2 * r; // two bytes per piece
	matrix.bitmap(4, 0, 4, 4, orientation, &pieces[offset]);
	
	
	/*
	for(int16_t iy = y; iy < y + height; iy++){
		for(int16_t ix = x; ix < x + width; ix++){
			if (pgm_read_byte_near(bitmap + byteCounter) & _BV(bitCounter)){
				state[ix][iy] = piece;
			}
			if (bitCounter == 0){
				byteCounter++;
				bitCounter = 8;
			}
			bitCounter--;
		}
	}
	
	flush();
	*/
}

void Tetris::rotate() {
	
}

void Tetris::down() {
	
}

void Tetris::left() {
	
}

void Tetris::right() {
	
}