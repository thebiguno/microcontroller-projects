#ifndef HEX_STATUS
#define HEX_STATUS

#include <avr/io.h>

void status_flash(uint8_t red, uint8_t green, uint8_t blue, uint8_t count);
void status_init();
void status_enable_timer();
void status_disable_timer();
void status_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif
