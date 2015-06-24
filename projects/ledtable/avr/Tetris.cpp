#include <Tetris.h>


Tetris::Tetris() {
}

Tetris::~Tetris() {
}

Tetris::run() {
	uint8_t running = 1;
	newGame();
	while (running) {
		generate();
		
		void psx_read_gamepad();
		buttons = psx_buttons();
		if (buttons & PSB_PAD_UP) rotate();
		else if (buttons & PSB_PAD_DOWN) down();
		else if (buttons & PSB_PAD_LEFT) left();
		else if (buttons & PSB_PAD_RIGHT) right();
		else if (buttons & PSB_TRIANGLE) running = 0;
		
		
	}
}