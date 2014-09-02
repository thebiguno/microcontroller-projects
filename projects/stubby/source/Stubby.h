#ifndef STUBBY_H
#define STUBBY_H

#define PCB_REVISION		1

#define LEG_COUNT			6

#define FRONT_LEFT			0
#define MIDDLE_LEFT			1
#define REAR_LEFT			2
#define REAR_RIGHT			3
#define MIDDLE_RIGHT		4
#define FRONT_RIGHT			5

#define JOINT_COUNT			3

#define TIBIA				0
#define FEMUR				1
#define COXA				2

#define LED_RED 			((LEG_COUNT * JOINT_COUNT) + 0)
#define LED_GREEN 			((LEG_COUNT * JOINT_COUNT) + 1)
#define LED_BLUE 			((LEG_COUNT * JOINT_COUNT) + 2)

#define PWM_COUNT			((LEG_COUNT * JOINT_COUNT) + 3)

#define CALIBRATION_COUNT	(JOINT_COUNT + 3)
#define CALIBRATION_X		(JOINT_COUNT + 0)
#define CALIBRATION_Y		(JOINT_COUNT + 1)
#define CALIBRATION_Z		(JOINT_COUNT + 2)

//PC / Stubby-specific messages are in 0x2X space...
#define MESSAGE_REQUEST_POWER_ON				0x20
#define MESSAGE_REQUEST_POWER_OFF				0x21
#define MESSAGE_REQUEST_POWER_TOGGLE			0x22
#define MESSAGE_REQUEST_MOVE					0x23
#define MESSAGE_REQUEST_TURN					0x24
#define MESSAGE_REQUEST_TRANSLATE				0x25
#define MESSAGE_REQUEST_ROTATE					0x26
#define MESSAGE_REQUEST_DISTANCE				0x27
#define MESSAGE_SEND_DISTANCE					0x28
#define MESSAGE_REQUEST_OPTICAL					0x29
#define MESSAGE_SEND_OPTICAL					0x2A
#define MESSAGE_REQUEST_SET_LED					0x2B


void servo_init();

void mode_select();
void mode_remote_control();
void mode_calibration();
	
#endif
