#ifndef TIMER1_H
#define TIMER1_H

#include <avr/interrupt.h>

void timer1_init();
void timer1_wait_for_tick();

#endif