#include "../main.h"

/*
 * given gyro_data in rad/s, accel_data in m/s/s, and delta t in seconds, return an attitude in radians (relative to the power on state)
 */
struct vector attitude(struct vector gyro, struct vector accel, double dt);