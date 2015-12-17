/*
 * ADC code to read battery.
 */
#ifndef BATTERY_H
#define BATTERY_H

#define BATTERY_DAMAGE_LEVEL		163
#define BATTERY_WARNING_LEVEL		175

#include <avr/io.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * Init the ADC
 */
void battery_init();
	
/*
 * Read the raw battery level.
 */
uint8_t battery_read();

#if defined (__cplusplus)
}
#endif

#endif
