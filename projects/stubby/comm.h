#ifndef HEX_COMM
#define HEX_COMM

#include <avr/io.h>
#include <avr/interrupt.h>

#include "lib/universal_controller/client.h"

#define LEG_COUNT	6
#define JOINT_COUNT	2

#include "lib/serial/serial.h"

#define SPECIAL_RESET		0x01

void comm_init();

void comm_read(double *velocity, double *direction, uint8_t *special);

#endif