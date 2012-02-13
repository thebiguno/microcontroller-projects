#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>

/*
 * Writes to a 4 digit 7-segment display.
 * c must have at least 4 chars.  Valid values are 0x0 ~ 0xF, and the chars 1234567890 abcdefghjlnoprtuy ABCDEFGHJLNOPRTUY -_=\/
 * flag bits 0 - 4 are the four decimal points
 * flag bit 5 is L1L2
 * flag bit 6 is L3
 */
void segment_draw(char c[], uint8_t flags);

#endif
