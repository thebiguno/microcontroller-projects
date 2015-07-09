#include "State.h"
#include "Display.h"

using namespace digitalcave;

State::State() :
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
	if (!button.read())
		this->state |= 0x80;
}

uint8_t State::get_state(){
	return this->state;
}
