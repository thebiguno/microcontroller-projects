#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>

/*
 * Translates a character into segments.
 * c must have at least 4 chars.  Valid values are 0x0 ~ 0xF, and the chars 1234567890 abcdefghjlnoprtuy ABCDEFGHJLNOPRTUY -_=\/
 */
uint8_t segment_lookup(char c);

#endif
