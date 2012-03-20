#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>

/*
 * Translates a character into segments.
 * Valid values are the chars sp 1234567890 abcdefghjlnoprtuy ABCDEFGHJLNOPRTUY -_=\/
 * Case is ignored.
 */
uint8_t segment_character(char c);

/*
 * Translates values from 0 to 9 into segmentsp
 */
uint8_t segment_decimal(uint8_t v);

/*
 * Turn on the decimal point.
 */
uint8_t segment_dp(uint8_t v);

#endif
