#ifndef PROCESSING_H
#define PROCESSING_H

#include <avr/wdt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "Stubby.h"
#include "Leg.h"
#include "gait/gait.h"
#include "hardware/magnetometer.h"
#include "hardware/status.h"
#include "util/convert.h"

//PC / Stubby-specific messages are in 0x2X space...
#define MESSAGE_REQUEST_POWER_ON				0x20
#define MESSAGE_REQUEST_POWER_OFF				0x21
#define MESSAGE_REQUEST_MOVE					0x22
#define MESSAGE_REQUEST_TURN					0x23
#define MESSAGE_REQUEST_TRANSLATE				0x24
#define MESSAGE_REQUEST_ROTATE					0x25
#define MESSAGE_REQUEST_HEADING					0x26
#define MESSAGE_SEND_HEADING					0x27
#define MESSAGE_REQUEST_DISTANCE				0x28
#define MESSAGE_SEND_DISTANCE					0x29
#define MESSAGE_REQUEST_OPTICAL					0x2A
#define MESSAGE_SEND_OPTICAL					0x2B
#define MESSAGE_REQUEST_SET_LED					0x2C

/*
 * Main processing method, where we look for incoming commands and perform actions as requested.
 */
void processing_command_executor();

/*
 * Processing-specific message dispatching.  This is called from an ISR, so keep things as short as possible here.
 */
void processing_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length);

#endif