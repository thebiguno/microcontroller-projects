#include "Button.h"

using namespace digitalcave;

#define STATE_PRESS			0x01
#define STATE_RELEASE		0x02
#define STATE_LONGPRESS		0x04
#define STATE_LONGRELEASE	0x08
#define STATE_REPEATPRESS	0x10

Button::Button(uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime) :
	pressTime(pressTime),
	releaseTime(releaseTime),
	longPressTime(longPressTime),
	repeatPressTime(repeatPressTime),
	pressCounter(0x00),
	releaseCounter(0xFFFF),
	longPressCounter(0x00),
	repeatPressCounter(0x00){
}

uint8_t Button::sample(uint32_t time) {
	uint16_t elapsedTime = time - lastTime;
	lastTime = time;

	uint8_t current = this->read();
	eventState = 0;

	if (current){	//If the button is currently pressed (not counting debouncing)...
		if (pressCounter == 0xFFFF){	//If the button has already passed the debounce threshold and has already fired a press event
			repeatPressCounter += elapsedTime;
			if (repeatPressCounter >= repeatPressTime){
				eventState |= STATE_REPEATPRESS;
				repeatPressCounter = 0;
			}

			if (longPressCounter < 0xFFFF){
				if (longPressCounter < longPressTime){
					longPressCounter += elapsedTime;
				}
				else {
					longPressCounter = 0xFFFF;
					eventState |= STATE_LONGPRESS;
					pressedState = 2;
				}
			}

			pressedState = 1;
		}
		else {	//If the button has not yet passed the debounce threshold
			pressCounter += elapsedTime;
			if (pressCounter >= pressTime){	//We are just now passing the debounce threshold; fire the press event
				eventState |= STATE_PRESS;
				pressCounter = 0xFFFF;
				releaseCounter = 0x00;
				longPressCounter = 0x00;
				repeatPressCounter = 0x00;
				pressedState = 1;
			}
		}
	}
	else {	//The button is currently not pressed (not counting debouncing)
		if (releaseCounter != 0xFFFF){
			releaseCounter += elapsedTime;
			if (releaseCounter >= releaseTime){	//We are just now passing the debounce (release) threshold; fire either the release or longrelease event depending on how long it has been pressed
				if (longPressCounter == 0xFFFF) eventState |= STATE_LONGRELEASE;
				else eventState |= STATE_RELEASE;
				releaseCounter = 0xFFFF;
				pressCounter = 0x00;
				longPressCounter = 0x00;
				repeatPressCounter = 0x00;
				pressedState = 0;
			}
		}
	}

	return pressedState;
}

uint8_t Button::getState() {
	return pressedState;
}

uint8_t Button::pressEvent() {
	return eventState & STATE_PRESS ? 1 : 0;
}

uint8_t Button::releaseEvent() {
	return eventState & STATE_RELEASE ? 1 : 0;
}

uint8_t Button::longPressEvent() {
	return eventState & STATE_LONGPRESS ? 1 : 0;
}

uint8_t Button::longReleaseEvent() {
	return eventState & STATE_LONGRELEASE ? 1 : 0;
}

uint8_t Button::repeatPressEvent() {
	return eventState & STATE_REPEATPRESS ? 1 : 0;
}


uint8_t Button::allPressEvent(Button* b1, Button* b2){
	return allPressEvent(b1, b2, NULL, NULL);
}
uint8_t Button::allPressEvent(Button* b1, Button* b2, Button* b3){
	return allPressEvent(b1, b2, b3, NULL);
}
uint8_t Button::allPressEvent(Button* b1, Button* b2, Button* b3, Button* b4){
	uint8_t count = 0;
	uint8_t pressed = 0;
	uint8_t pressEvent = 0;
	if (b1 != NULL){
		count++;
		if (b1->getState()) pressed++;
		if (b1->pressEvent()) pressEvent++;
	}
	if (b2 != NULL){
		count++;
		if (b2->getState()) pressed++;
		if (b2->pressEvent()) pressEvent++;
	}
	if (b3 != NULL){
		count++;
		if (b3->getState()) pressed++;
		if (b3->pressEvent()) pressEvent++;
	}
	if (b4 != NULL){
		count++;
		if (b4->getState()) pressed++;
		if (b4->pressEvent()) pressEvent++;
	}

	if (count == pressed && pressEvent > 0) return 1;
	else return 0;
}

uint8_t Button::allLongPressEvent(Button* b1, Button* b2){
	return allLongPressEvent(b1, b2, NULL, NULL);
}
uint8_t Button::allLongPressEvent(Button* b1, Button* b2, Button* b3){
	return allLongPressEvent(b1, b2, b3, NULL);
}
uint8_t Button::allLongPressEvent(Button* b1, Button* b2, Button* b3, Button* b4){
	uint8_t count = 0;
	uint8_t longPressed = 0;
	uint8_t longPressEvent = 0;
	if (b1 != NULL){
		count++;
		if (b1->getState() == 2) longPressed++;
		if (b1->longPressEvent()) longPressEvent++;
	}
	if (b2 != NULL){
		count++;
		if (b2->getState() == 2) longPressed++;
		if (b2->longPressEvent()) longPressEvent++;
	}
	if (b3 != NULL){
		count++;
		if (b3->getState() == 2) longPressed++;
		if (b3->longPressEvent()) longPressEvent++;
	}
	if (b4 != NULL){
		count++;
		if (b4->getState() == 2) longPressed++;
		if (b4->longPressEvent()) longPressEvent++;
	}

	if (count == longPressed && longPressEvent > 0) return 1;
	else return 0;
}
