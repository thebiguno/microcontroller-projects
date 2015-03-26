#ifndef TIMER1_H
#define TIMER1_H

#include <avr/interrupt.h>

void timer1_init();
uint8_t timer1_next_tick();

#endif