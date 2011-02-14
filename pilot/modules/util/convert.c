#include "convert.h"

union udouble {
	double d;
	uint8_t u[sizeof(double)];
};

double convert_byte_to_radian(uint8_t x)
{
	double r = 0.024639942381096 * x;
	return (r > M_PI) ? r - (2.0 * M_PI) : r; // convert quadrants III & IV into negative values
}
uint8_t convert_radian_to_byte(double x)
{
	double r = (x < 0) ? (2.0 * M_PI) + x : x; // convert quadrants III & IV into positive values
	return (uint8_t) (40.584510488433314 * r);
}

double convert_byte_to_percent(uint8_t x)
{
	return 0.003921568627451 * x;
}
uint8_t convert_percent_to_byte(double x)
{
	return (uint8_t) (255 * x);
}

void convert_double_to_bytes(double value, uint8_t *buffer, uint8_t offset)
{
    union udouble converter;
    converter.d = value;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        buffer[i + offset] = converter.u[i];
    }
}
double convert_bytes_to_double(uint8_t *buffer, uint8_t offset)
{
    union udouble converter;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        converter.u[i] = buffer[i + offset];
    }
    return converter.d;
}

double convert_byte_to_throttle(int8_t x) {
	return (0.007874015748031f * x) - 1;
}
uint8_t convert_throttle_to_byte(double x) {
	return (int8_t) (127 * x) + 127;
}
