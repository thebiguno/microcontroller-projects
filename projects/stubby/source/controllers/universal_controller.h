#ifndef UNIVERSAL_CONTROLLER_CLIENT_STUBBY_H
#define UNIVERSAL_CONTROLLER_CLIENT_STUBBY_H

#include "lib/universal_controller/client.h"
#include "lib/pwm/pwm.h"
#include "hardware/status.h"
#include "types/Point.h"
#include "Stubby.h"
#include "Leg.h"

#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>

/*
 * Main entry point to the remote control code.  When controller is set to UC, this method is called from main().
 */
void uc_command_executor();

/*
 * Called from the Stubby protocol dispatch function; handles the UC-specific dispatching
 */
void uc_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length);

#endif