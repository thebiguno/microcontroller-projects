#include "universal_controller.h"

static volatile uc_stick_t left_stick;
static volatile uc_stick_t right_stick;

static uint16_t uc_buttons_pressed;
static uint16_t uc_buttons_released;
static uint16_t uc_buttons_held;

uint16_t uc_read_pressed_buttons(){
	if (uc_buttons_pressed == 0x00) return 0x00;
	
	uint16_t result = uc_buttons_pressed;
	uc_buttons_pressed = 0x00;
	return result;
}

uint16_t uc_read_released_buttons(){
	if (uc_buttons_released == 0x00) return 0x00;
	
	uint16_t result = uc_buttons_released;
	uc_buttons_released = 0x00;
	return result;
}

uint16_t uc_read_held_buttons(){
	return uc_buttons_held;
}

uc_stick_t uc_read_left(){
	uc_stick_t result;
	result.x = left_stick.x;
	result.y = left_stick.y;
	return result;
}
uc_stick_t uc_read_right(){
	uc_stick_t result;
	result.x = right_stick.x;
	result.y = right_stick.y;
	return result;
}

void uc_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	//Universal Controller buttons / joysticks
	if (cmd == MESSAGE_UC_BUTTON_PUSH){
		uint16_t pressed_value = _BV(message[0]);
		uc_buttons_pressed |= pressed_value;	//Mark as pressed; this is cleared when they are read.
		uc_buttons_held |= pressed_value;		//Mark as held; this is cleared when released
	}
	else if (cmd == MESSAGE_UC_BUTTON_RELEASE){
		uint16_t released_value = _BV(message[0]);
		uc_buttons_released |= released_value;	//Mark as released; this is cleared when they are read.
		uc_buttons_held &= ~released_value;		//Mark as no longer held
	}
	else if (cmd == MESSAGE_UC_JOYSTICK_MOVE){
		left_stick.x = message[0] - 127;
		left_stick.y = 127 - message[1];
		right_stick.x = message[2] - 127;
		right_stick.y = 127 - message[3];
	}
}