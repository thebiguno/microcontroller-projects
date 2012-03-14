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
 * Translates hexadecimal into alternative digits.
 * http://mackwai.blogspot.com/2009/07/alternative-hexadecimal-digits.html
 */
uint8_t segment_hexadecimal(uint8_t v);

/*
 * Translates decimal into their normal Arabic-Indic representations.
 * Also supports X for Ten and E for Eleven for duodecimal.
 */
uint8_t segment_decimal(uint8_t v);

/*
 * Translates vigesimal into alternative digits.
 * The horizontal bars are 5, 10, and 15 markers. The vertical bars are 1, 2, 3, 4.
 */
uint8_t segment_vigesimal(uint8_t v);

/*
 * Turn on the decimal point.
 */
uint8_t segment_dp(uint8_t v);

#endif
