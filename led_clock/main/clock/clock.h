#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

#define CLOCK_MODES 7

/*
 * Sets the clock mode.
 * 0: traditional (24:59:59)
 * 1: vigesimal(19:19:19:19)
 * 2: hexadecimal (F:F:F:F)
 * 3: decimal (9:9:9:9:9)
 */
void clock_mode(uint8_t mode);

/*
 * Recalculates the segment and matrix state.
 */
void clock_update(uint32_t millis);

/*
 * Gets the four segments according to the time in the given mode.
 */
void clock_segments(uint8_t result[]);

/*
 * Gets the segment flags to the time in the given mode.
 * Bit 1 is the colon.  Bit 2 is the extra dot.
 */
uint8_t clock_segment_flags();

/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_matrix(uint8_t red[], uint8_t grn[]);

uint32_t clock_size_b();
uint32_t clock_size_d();

#endif