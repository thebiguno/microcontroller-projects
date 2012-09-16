#ifndef MAIN_H
#define MAIN_H

#define GIT_VERSION		"1.0.0-2-g826fc6f"

#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
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

typedef struct vector {
    double x;
    double y;
    double z;
} vector_t;


uint16_t dt;

#include "modules/battery/battery.h"
#include "modules/control/control.h"
#include "modules/comm/communication.h"
#include "modules/status/status.h"
#include "modules/protocol/protocol.h"

#endif
