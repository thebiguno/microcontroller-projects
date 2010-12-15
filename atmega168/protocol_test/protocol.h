#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <math.h>

union udouble {
	double d;
	uint8_t u[sizeof(double)];
};

void protocol_send_message(uint8_t cmd, uint8_t *bytes, uint8_t length);

double protocol_byte_to_radian(uint8_t x);
uint8_t protocol_radian_to_byte(double x);
double protocol_byte_to_percent(uint8_t x);
uint8_t protocol_percent_to_byte(double x);
void protocol_double_to_bytes(double value, uint8_t *buf, uint8_t offset);
double protocol_bytes_to_double(uint8_t *buf, uint8_t offset);

#endif
