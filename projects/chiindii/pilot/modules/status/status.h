#ifndef STATUS_H
#define STATUS_H

#include "../../main.h"

#define STATUS_HEARTBEAT	_BV(0)
#define STATUS_ARMED		_BV(1)
#define STATUS_MESSAGE_TX	_BV(2)
#define STATUS_MESSAGE_RX	_BV(3)

#define STATUS_ERR_GYRO_CHECKSUM 		0x01
#define STATUS_ERR_ACCEL_CHECKSUM 		0x02
#define STATUS_ERR_ATTITUDE_CHECKSUM 	0x03
#define STATUS_ERR_PID_CHECKSUM 		0x04
#define STATUS_ERR_EEPROM				0x05
#define STATUS_ERR_PROTOCOL 			0x06
#define STATUS_ERR_RESET				0X0F

void status_init();
void status_set(uint8_t bv);
void status_clear(uint8_t bv);
void status_toggle(uint8_t bv);

/*
 * Turns the error LED on and sets the other LEDs according to the code (a value between 0x1 and 0xF)
 * Use a value of 0x0 to clear the error state and resume normal error status reporting
 * Conditions:
 */
void status_error(uint8_t condition);

#endif
