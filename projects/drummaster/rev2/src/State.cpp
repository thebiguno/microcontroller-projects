#include "State.h"

using namespace digitalcave;

State::State() :
	hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7),
	char_display(&this->hd44780, DISPLAY_ROWS, DISPLAY_COLS),
	encoder(ENC_A, ENC_B),
	button(ENC_PUSH, 25)
{
	pinMode(ENC_PUSH, INPUT_PULLUP);	//Encoder pushbutton
}

void State::poll(){
	button.update();
	int8_t encoder_state = encoder.read();
	if (encoder_state < 0) encoder.write(ENCODER_STATE_COUNT - 1);
	else if (encoder_state >= ENCODER_STATE_COUNT) encoder.write(0);

	this->state = encoder.read() >> 1;

	char buf[21];
	if (this->state == 1 && this->button.fallingEdge()){
		copyFolderToFlash("/", char_display);
		
		this->state = 0;
	}
	else {
		snprintf(buf, sizeof(buf), "State %d                    ", (uint8_t) this->get_state());
		char_display.write_text(0, 0, buf, sizeof(buf));
	}
	
	char_display.refresh();
}

uint8_t State::get_state(){
	return this->state;
}
