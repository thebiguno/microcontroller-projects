#include "convert.h"

union udouble {
	double d;
	uint8_t u[sizeof(double)];
};

void convert_double_to_bytes(double value, uint8_t *buffer, uint8_t offset){
	union udouble converter;
	converter.d = value;
	for (uint8_t i = 0; i < sizeof(double); i++) {
		buffer[i + offset] = converter.u[i];
	}
}
double convert_bytes_to_double(uint8_t *buffer, uint8_t offset){
	union udouble converter;
	for (uint8_t i = 0; i < sizeof(double); i++) {
		converter.u[i] = buffer[i + offset];
	}
	return converter.d;
}

double convert_byte_to_radian(uint8_t x){
	double r = 0.024639942381096 * x;
	return normalize_angle(r);
}
uint8_t convert_radian_to_byte(double r){
	r = normalize_angle(r);
	return (uint8_t) (40.584510488433314 * r);
}

double normalize_angle(double radians){
	while (radians > M_PI){
		radians += (M_PI * -2);
	}
	while (radians < (M_PI * -1)){
		radians += (M_PI * 2);
	}
	return radians;
}

double difference_between_angles(double a1, double a2){
	return normalize_angle(atan2(sin_f(a2), cos_f(a2)) - atan2(sin_f(a1), cos_f(a1)));
}