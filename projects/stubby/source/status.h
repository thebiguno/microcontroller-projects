#ifndef HEX_STATUS
#define HEX_STATUS

#include <avr/io.h>

#include "Stubby.h"
#include "lib/pwm/pwm.h"

void status_init();
void status_enable_timer();
void status_disable_timer();
void status_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif
