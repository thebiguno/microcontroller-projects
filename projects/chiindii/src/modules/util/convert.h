#ifndef CONVERT_H
#define CONVERT_H

#include "../../main.h"

void convert_double_to_bytes(double value, uint8_t *buf, uint8_t offset);
double convert_bytes_to_double(uint8_t *buf, uint8_t offset);

double convert_byte_to_radian(uint8_t x);
uint8_t convert_radian_to_byte(double x);

double convert_byte_to_percent(uint8_t x);
uint8_t convert_percent_to_byte(double x);

double convert_byte_to_throttle(int8_t x);
uint8_t convert_throttle_to_byte(double x);

#endif
