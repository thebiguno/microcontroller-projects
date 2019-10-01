#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

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

#include "Light.h"
#include "Encoder.h"
#include "Sound.h"

//The start of the EEPROM
#define EEPROM_CALIBRATION_OFFSET		0x00

//The minimum the light setpoint can go.  This should equal PWM_MIN / PWM_MAX (from Light.cpp)
#define MIN_LIGHT						0.25

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.

//Time mode (default state).  Shows the time.  Press the button to toggle light, turn encoder to
// adjust light brightness.  Hold button to enter menu.
#define MODE_TIME						0x00

#define EDIT_TIME_TIME					0x00
#define EDIT_TIME_LAMP					0x01
#define EDIT_TIME_MUSIC					0x02
#define EDIT_TIME_DATE					0x03

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
	dc_time_t time;
	uint8_t enabled;		//Bit mask for which days are enabled.  Defined as _BV(DAY), where DAY 0 is Sunday, 1 is Monday, etc.  _BV(7) is the global enabled flag, allowing you to turn off an alarm without changing the days it is active for.
	uint8_t lamp_speed;		//Time to go from 0 to full brightness, in minutes
	uint8_t music_speed;	//Time to go from 0 to music_volume, in minutes
	uint8_t music_volume;	//Max volume of music, reached after music_speed minutes
} alarm_t;

namespace digitalcave {

	class State {
		private:
			I2CAVR i2c;
			DS3231 calendar;
			Encoder encoder;
			ButtonAVR lampButton;
			ButtonAVR musicButton;

			//Alarm stuff
			alarm_t alarm[ALARM_COUNT];			//The actual alarms
			uint8_t alarm_triggered = 0;		//_BV(alarm_index) is set when alarm[alarm_index] is triggered.  If we make any changes to the light, this is reset to 0.  When it is non-zero, we incrememnt light / music gradually.
			double light_brightness = 0;		//Keep track of light brightness...
			double light_color = 0;				//... and light color temperature

			//General stuff
			uint8_t display_brightness = 0;		//The brightness for the LED matrix.  0 - 15.

			//Stuff for menus
			uint8_t mode = 0;					//Main modes.  TIME, MENU, EDIT
			uint8_t menu_item = 0;	//From 0 to MENU_COUNT - 1.  The currently selected menu item.
			uint8_t edit_item = 0;	//Functionality depends on edit item.  Stuff like setting times and alarms.

		public:
			State();

			void poll();

			dc_time_t get_time();

			alarm_t get_alarm(uint8_t index);

			uint8_t get_display_brightness();
			double get_lamp_brightness();

			uint8_t get_mode();
			uint8_t get_menu_item();
			uint8_t get_edit_item();
	};
}

#endif
