#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <math.h>

typedef struct control_t {
    double pitch;
    double roll;
    double yaw;
    double throttle;
} control_t;

#include "modules/control/control.h"
#include "modules/communication/communication.h"

#endif
