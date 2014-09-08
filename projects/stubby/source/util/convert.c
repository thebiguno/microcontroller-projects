#include "convert.h"

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
