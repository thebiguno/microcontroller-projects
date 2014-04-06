#ifndef HEX_MAIN
#define HEX_MAIN

#include <avr/io.h>

#define LEG_COUNT	6
#define JOINT_COUNT	2

#include "lib/serial/serial.h"
#include "ripple.h"
#include "servo.h"
#include "leg.h"

/*
 * Look for any serial communications, and read all that are available.  Set 
 * the command state from this data.
 */
void hex_read_serial_command();

double get_velocity();

double get_turn();

#endif