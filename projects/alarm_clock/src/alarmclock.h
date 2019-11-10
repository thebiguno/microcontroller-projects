#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include <timer/timer.h>
#include <max7219.h>

#ifdef DEBUG
#include <SerialUSB.h>
#endif

#include "display.h"
#include "music.h"
#include "state.h"

#endif
