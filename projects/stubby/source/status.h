#ifndef HEX_STATUS
#define HEX_STATUS

#include <avr/io.h>

#include "stubby.h"
#include "lib/pwm/pwm.h"

#define LED_RED (LEG_COUNT * JOINT_COUNT) + 0
#define LED_GREEN (LEG_COUNT * JOINT_COUNT) + 1
#define LED_BLUE (LEG_COUNT * JOINT_COUNT) + 2

void status_init();
void status_enable_timer();
void status_disable_timer();
void status_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif
