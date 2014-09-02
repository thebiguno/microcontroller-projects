#ifndef UNIVERSAL_CONTROLLER_CLIENT_H
#define UNIVERSAL_CONTROLLER_CLIENT_H

//Universal Controller messages are in 0x1X space...
#define MESSAGE_UC_BUTTON_PUSH					0x10
#define MESSAGE_UC_BUTTON_RELEASE				0x11
#define MESSAGE_UC_BUTTON_NONE					0x12
#define MESSAGE_UC_JOYSTICK_MOVE				0x13
#define MESSAGE_UC_BUTTON_ENABLE				0x14
#define MESSAGE_UC_BUTTON_DISABLE				0x15
#define MESSAGE_UC_JOYSTICK_ENABLE				0x16
#define MESSAGE_UC_JOYSTICK_DISABLE				0x17
#define MESSAGE_UC_SET_POLL_FREQUENCY			0x18
#define MESSAGE_UC_SET_ANALOG_FREQUENCY			0x19



//Bit 8: 0 == analog stick message, 1 == digital button message
#define CONTROLLER_MESSAGE_TYPE_MASK	0x80
#define CONTROLLER_MESSAGE_TYPE_ANALOG	0x00
#define CONTROLLER_MESSAGE_TYPE_BUTTON	0x80


//Bit 7 (Analog): Left / Right stick
#define CONTROLLER_ANALOG_STICK			0x40
#define CONTROLLER_ANALOG_STICK_LEFT	0x00
#define CONTROLLER_ANALOG_STICK_RIGHT	0x40

//Bit 6 (Analog): X / Y axis
#define CONTROLLER_ANALOG_AXIS			0x20
#define CONTROLLER_ANALOG_AXIS_X		0x00
#define CONTROLLER_ANALOG_AXIS_Y		0x20

//Bit 5::1 (Analog): Value
#define CONTROLLER_ANALOG_VALUE_MASK	0x1F

//No Buttons Pressed message
#define CONTROLLER_BUTTON_NONE			0xC0

//Bit 5 (Button): Press / Release
#define CONTROLLER_BUTTON_PRESS_MASK	0x10
#define CONTROLLER_BUTTON_RELEASE		0x00
#define CONTROLLER_BUTTON_PRESS			0x10

//Bit 4::1 (Button): Button address
#define CONTROLLER_BUTTON_VALUE_MASK	0x0F

#define CONTROLLER_BUTTON_VALUE_SELECT		0x00
#define CONTROLLER_BUTTON_VALUE_LEFT3		0x01
#define CONTROLLER_BUTTON_VALUE_RIGHT3		0x02
#define CONTROLLER_BUTTON_VALUE_START		0x03
#define CONTROLLER_BUTTON_VALUE_PADUP		0x04
#define CONTROLLER_BUTTON_VALUE_PADRIGHT	0x05
#define CONTROLLER_BUTTON_VALUE_PADDOWN		0x06
#define CONTROLLER_BUTTON_VALUE_PADLEFT		0x07
#define CONTROLLER_BUTTON_VALUE_LEFT2		0x08
#define CONTROLLER_BUTTON_VALUE_RIGHT2		0x09
#define CONTROLLER_BUTTON_VALUE_LEFT1		0x0A
#define CONTROLLER_BUTTON_VALUE_RIGHT1		0x0B
#define CONTROLLER_BUTTON_VALUE_TRIANGLE	0x0C
#define CONTROLLER_BUTTON_VALUE_CIRCLE		0x0D
#define CONTROLLER_BUTTON_VALUE_CROSS		0x0E
#define CONTROLLER_BUTTON_VALUE_SQUARE		0x0F

#include <avr/io.h>

typedef struct uc_stick_t {
	int8_t x;
	int8_t y;
} uc_stick_t;

void uc_init();

/*
 * Read all buttons which have been pressed since the last
 * call to uc_read_pressed_buttons().
 */
uint16_t uc_read_pressed_buttons();

/*
 * Read all buttons which have been released since the last
 * call to uc_read_released_buttons();
 */
uint16_t uc_read_released_buttons();

/*
 * Read all buttons which are currently being held.
 */
uint16_t uc_read_held_buttons();

/*
 * Read the left stick (x and y).  0 is neutral, negative 
 * numbers are left / down, positive numbers are right / up.
 */
uc_stick_t uc_read_left();

/*
 * Read the right stick (x and y).  0 is neutral, negative 
 * numbers are left / down, positive numbers are right / up.
 */
uc_stick_t uc_read_right();

#endif