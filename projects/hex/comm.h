#ifndef HEX_COMM
#define HEX_COMM

#include <avr/io.h>

#define LEG_COUNT	6
#define JOINT_COUNT	2

#include "lib/serial/serial.h"

void comm_init();

void comm_read(double *velocity, double *direction);

#endif