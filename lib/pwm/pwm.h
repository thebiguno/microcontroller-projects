#ifndef PWM_H
#define PWM_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include "../serial/serial.h"

void pwm_init(volatile uint8_t *ports[],
				volatile uint8_t *ddrs[],
				uint8_t pins[],
				uint8_t count,
				uint16_t period);
				
				
/*
 * Sets the phase of the pin at the given index to the specified value.
 */
void pwm_set(uint8_t index, uint16_t phase);


#endif
