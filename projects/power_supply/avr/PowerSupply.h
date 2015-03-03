#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <avr/io.h>
#include <util/delay.h>

#include "Channel.h"
#include "Display.h"
#include "State.h"
#include "usb.h"

#ifndef CHANNEL_COUNT
#define CHANNEL_COUNT					2
#endif

#if CHANNEL_COUNT > 6
#error The software does not currently support more than 6 channels (not enough ADCs on an ATmega32u4).
#endif



#endif