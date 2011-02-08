#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <math.h>
#include "../lib/timer/timer.h"

//Figure out which pin mapping to use
#if defined(__AVR_ATmega168__) || \
	defined(__AVR_ATmega328P__) 
#include "pins/328.h"
#elif defined(__AVR_ATmega644__) || \
	defined(__AVR_ATmega644P__) || \
	defined(__AVR_ATmega644PA__)
#include "pins/644.h"
#else
#error You must define the pin mappings for your chip!  Please verify that MMCU is set correctly, and that there is a matching pin definition file declared at the top of main.h
#endif

typedef struct control {
    double pitch;
    double roll;
    double yaw;
    double throttle;
} control_t;

#include "modules/control/control.h"
#include "modules/comm/communication.h"
#include "modules/protocol/protocol.h"

#endif
