#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>

//Figure out which pin mapping to use
#if defined(__AVR_ATmega168__) || \
	defined(__AVR_ATmega328P__) 
	#include "pins328.h"
#elif defined(__AVR_ATmega644__) || \
	defined(__AVR_ATmega644P__) || \
	#include "pins644.h"
#endif 

void set_data(char *data);

void send_debug_data(uint8_t *data);

typedef struct vector {
    double x;
    double y;
    double z;
} vector_t;

//Common modules which other modules need to interact with
#include "modules/persist/persist.h"

//Needed to push data out to the modules
#include "modules/comm/communication.h"
#include "modules/accel/accel.h"
#include "modules/gyro/gyro.h"
#include "modules/attitude/attitude.h"
#include "modules/motor/motor.h"
#include "modules/esc/esc.h"
#include "modules/pid/pid.h"

#endif
