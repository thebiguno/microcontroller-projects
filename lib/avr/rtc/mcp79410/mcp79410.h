#ifndef mcp79410_h
#define mcp79410_h

#include <stdint.h>

/* This library assumes the following
 * Crystal oscilator (ST=1, EXTOSC=0)
 * Battery backup is in use (VBATEN=1)
 */

#if defined (__cplusplus)
extern "C" {
#endif
	
struct mcp79410_time_t {
	uint8_t  second;
	uint8_t  minute;
	uint8_t  hour;
	uint8_t  wday;
	uint8_t  mday;
	uint8_t  month;
	uint8_t  year;
};

void mcp79410_get(struct mcp79410_time_t *time);
void mcp79410_set(struct mcp79410_time_t *time);

/* enable square wave output at 1 Hz (disables alarm interrupt) */
void mcp79410_set_1hz();
/* enable square wave output at 4.096 kHz (disables alarm interrupt) */
void mcp79410_set_4khz();
/* enable square wave output at 8.192 kHz hz (disables alarm interrupt) */
void mcp79410_set_8khz();
/* enable square wave output at 8.192 kHz hz  (disables alarm interrupt) */
void mcp79410_set_32khz();
/* set multifunction pin low */
void mcp79410_set_mfp_low();
/* set multifunction pin high */
void mcp79410_set_mfp_high();

uint8_t mcp79410_read_sram(uint8_t offset);
void mcp79410_write_sram(uint8_t data, uint8_t offset);

void mcp79410_set_trim(int8_t trim);

#if defined (__cplusplus)
}
#endif

#endif
