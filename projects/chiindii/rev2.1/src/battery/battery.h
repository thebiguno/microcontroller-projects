/*
 * ADC code to read battery.
 *
 * damage voltage = 3V		= 163
 * warning voltage = 3.2V	= 175
 * full voltage = 4.2V		= 229
 */
#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

#define BATTERY_DAMAGE_LEVEL		163
#define BATTERY_WARNING_LEVEL		175
#define BATTERY_FULL_LEVEL			229

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

/*
 * Returns the ADC value scaled to a percentage value between 0 and 100
 * where 0 is the battery damage level, 100 is the battery full level, and 20 is the battery warning level
 */
uint8_t battery_pct();

#if defined (__cplusplus)
}
#endif

#endif
