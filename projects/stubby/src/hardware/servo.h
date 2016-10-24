#ifndef SERVO_H
#define SERVO_H

#include <avr/eeprom.h>

#include "../Leg.h"
#include "../hardware/status.h"

#include "../hardware/pwm.h"

void servo_init(Leg** legs);

#endif
