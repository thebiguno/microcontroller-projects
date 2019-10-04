#ifndef ENCODER_H
#define ENCODER_H

#include <avr/interrupt.h>

#if defined (__cplusplus)
extern "C" {
#endif

void encoder_init();

int8_t encoder_get_movement_1();
int8_t encoder_get_movement_2();

#if defined (__cplusplus)
}
#endif

#endif
