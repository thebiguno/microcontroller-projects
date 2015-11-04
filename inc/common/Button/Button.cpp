#include "Button.h"

using namespace digitalcave;

#define STATE_PRESS			0x01
#define STATE_RELEASE		0x02
#define STATE_LONGPRESS		0x04
#define STATE_LONGRELEASE	0x08
#define STATE_REPEATPRESS	0x10

void Button::init(uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime){
	this->pressTime = pressTime;
	this->releaseTime = releaseTime;
	this->longPressTime = longPressTime;
	this->repeatPressTime = repeatPressTime;
	
	this->pressCounter = 0x00;
	this->releaseCounter = 0xFFFF;
	this->longPressCounter = 0x00;
	this->repeatPressCounter = 0x00;
}

uint8_t Button::sample(uint32_t time) {
	uint16_t elapsedTime = time - lastTime;
	lastTime = time;

	uint8_t current = this->read();
	state = 0;
	
	if (current){
		if (pressCounter == 0xFFFF){
			repeatPressCounter += elapsedTime;
			if (repeatPressCounter >= repeatPressTime){
				state |= STATE_REPEATPRESS;
				repeatPressCounter = 0;
			}
			
			if (longPressCounter < 0xFFFF){
				if (longPressCounter < longPressTime){
					longPressCounter += elapsedTime;
				}
				else {
					longPressCounter = 0xFFFF;
					state |= STATE_LONGPRESS;
				}
			}
			
			return 1;
		}
		else {
			pressCounter += elapsedTime;
			if (pressCounter >= pressTime){
				state |= STATE_PRESS;
				pressCounter = 0xFFFF;
				releaseCounter = 0x00;
				longPressCounter = 0x00;
				repeatPressCounter = 0x00;
				return 1;
			}
		}
	}
	else {
		if (releaseCounter != 0xFFFF){
			releaseCounter += elapsedTime;
			if (releaseCounter >= releaseTime){
				if (longPressCounter == 0xFFFF) state |= STATE_LONGRELEASE;
				else state |= STATE_RELEASE;
				releaseCounter = 0xFFFF;
				pressCounter = 0x00;
				longPressCounter = 0x00;
				repeatPressCounter = 0x00;
			}
		}
	}
	
	return 0;
}

uint8_t Button::pressEvent() {
	return state & STATE_PRESS;
}

uint8_t Button::releaseEvent() {
	return state & STATE_RELEASE;
}

uint8_t Button::longPressEvent() {
	return state & STATE_LONGPRESS;
}

uint8_t Button::longReleaseEvent() {
	return state & STATE_LONGRELEASE;
}

uint8_t Button::repeatPressEvent() {
	return state & STATE_REPEATPRESS;
}