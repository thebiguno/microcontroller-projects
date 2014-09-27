#ifndef CONVERT_H
#define CONVERT_H

#include <stdint.h>
#include <math.h>

double convert_byte_to_radian(uint8_t x);
uint8_t convert_radian_to_byte(double x);

void convert_double_to_bytes(double value, uint8_t *buf, uint8_t offset);
double convert_bytes_to_double(uint8_t *buf, uint8_t offset);

/*
 * Normalize an angle (in radians) such that it is always in the range from
 * -M_PI to M_PI.
 */
double normalize_angle(double radians);

#endif
