#ifndef SERVO_H
#define SERVO_H

#include <avr/eeprom.h>
#include "lib/pwm/pwm.h"
#include "hardware/status.h"
#include "Leg.h"

void servo_init(Leg *legs);

#endif