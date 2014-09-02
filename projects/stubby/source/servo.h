#ifndef SERVO_H
#define SERVO_H

#include <util/delay.h>
#include <avr/eeprom.h>
#include "lib/pwm/pwm.h"
#include "Leg.h"

void servo_init(Leg *legs);

#endif