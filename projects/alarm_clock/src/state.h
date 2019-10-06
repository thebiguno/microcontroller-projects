#ifndef STATE_H
#define STATE_H

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <dctime.h>
#include <avr/eeprom.h>

#include <bootloader/bootloader.h>
#include <Button/ButtonAVR.h>
#include <timer/timer.h>
#include <bootloader.h>
#include <I2CAVR.h>
#include <DS3231.h>

#ifdef DEBUG
#include <SerialUSB.h>
#endif

#include "light.h"
#include "encoder.h"
#include "music.h"

//The start of the EEPROM
#define EEPROM_CALIBRATION_OFFSET		0x00

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.

//Time mode (default state).  Shows the time.  Press the button to toggle light, turn encoder to
// adjust light brightness.  Hold button to enter menu.
#define MODE_TIME						0x00

#define EDIT_TIME_TIME					0x00
#define EDIT_TIME_LAMP					0x01
#define EDIT_TIME_MUSIC					0x02

//Menu mode: allow setting time, alarm, etc.
#define MODE_MENU						0x01

//Edit whatever menu item you are currently in
#define MODE_EDIT						0x02

//The menu indices for various functions
#define MENU_SET_ALARM_1				0x00
#define MENU_SET_ALARM_2				0x01
#define MENU_SET_ALARM_3				0x02
#define MENU_SET_TIME					0x03
#define MENU_DFU						0x04

#define MENU_SIZE						0x05

#define ALARM_COUNT						3

typedef struct alarm {
	uint8_t hour;			//Hour for alarm (0-23)
	uint8_t minute;			//Minute for alarm (0-59)
	uint8_t enabled;		//Bit mask for which days are enabled.  Defined as _BV(DAY), where DAY 0 is Sunday, 1 is Monday, etc.  _BV(7) is the global enabled flag, allowing you to turn off an alarm without changing the days it is active for.
	uint8_t lamp_speed;		//Time to go from 0 to full brightness, in minutes
	uint8_t music_speed;	//Time to go from 0 to music_volume, in minutes
	uint8_t music_volume;	//Max volume of music, reached after music_speed minutes
} alarm_t;

void state_init();

void state_poll();

void state_get_time(time_t* time, tm_t* tm);

alarm_t state_get_alarm(uint8_t index);

uint8_t state_get_display_brightness();
double state_get_lamp_brightness();

uint8_t state_get_mode();
uint8_t state_get_menu_item();
uint8_t state_get_edit_item();

#endif
