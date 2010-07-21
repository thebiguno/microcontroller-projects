#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <math.h>

void set_data(char *data);

void send_debug_data(uint8_t *data);

typedef struct vector{
    double x;
    double y;
    double z;
} vector_t;

//Needed to push data out to the modules
/*
#include "modules/accel/accel.h"
#include "modules/attitude/attitude.h"
#include "modules/gyro/gyro.h"
#include "modules/motor/motor.h"
#include "modules/pid/pid.h"
*/

#endif
