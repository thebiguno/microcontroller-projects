#ifndef HEX_STATUS
#define HEX_STATUS

#include <avr/io.h>

//The minimum ADC value which indicates a full battery
#define BATTERY_LEVEL_FULL		180
//The maximum ADC value which indicates an empty battery
#define BATTERY_LEVEL_EMPTY		150

//The magic number 78 comes from X/20000 = color/255; X = color * 20000 / 255; X = color * 78.
#define PHASE_MULTIPLIER		(uint8_t) (20000/255)

//Defines a linear progression for valid battery values.  Includes the magic number 78 
// as calculated in 
#define BATTERY_STATUS_MULTIPLIER		255 / (BATTERY_LEVEL_FULL - BATTERY_LEVEL_EMPTY) * PHASE_MULTIPLIER

void status_flash(uint8_t red, uint8_t green, uint8_t blue, uint8_t count);
void status_init();
void status_enable_battery();
void status_disable_battery();
void status_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif
