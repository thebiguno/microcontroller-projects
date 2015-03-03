/*
 * A single power supply channel.  Includes all variables needed for setting DAC, ADC readings, 
 * formatting display, setting values, etc.
 */
#ifndef CHANNEL_H
#define CHANNEL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

#include "lib/twi/twi.h"

//TODO Change these to be instance variables set depending on the voltage divider resistor values
#define VOLTAGE_MULTIPLIER		3
#define CURRENT_MULTIPLIER		1

#define RUNNING_AVERAGE_COUNT	10

namespace digitalcave {

	class Channel {
		private:
			uint8_t i2c_address;
			uint8_t dac_bank;		//Bit 2 of the DAC channel.  i.e. 0 for DAC output A and B, 1 for DAC output C and D.
			
			//All voltage values are in millivolts
			int16_t voltage_max;
			int16_t voltage_setpoint = 0;
			int16_t voltage_actual = 0;
			uint8_t voltage_adc_channel;
			
			//All current values are in milliamps
			int16_t current_max;
			int16_t current_setpoint = 0;
			int16_t current_actual = 0;
			uint8_t current_adc_channel;
			
			static uint8_t selected_i2c_address_and_reading;	//Used to show menu cursor
			
		public:
			/*
			 * Initializes the channel
			 */
			Channel(uint8_t i2c_address, uint8_t dac_bank,
				int16_t voltage_max, int16_t current_max,
				uint8_t voltage_adc_channel, uint8_t current_adc_channel);


			int16_t get_voltage_actual();
			int16_t get_voltage_setpoint();
			void set_voltage_setpoint(int16_t setpoint);
			
			int16_t get_current_actual();
			int16_t get_current_setpoint();
			void set_current_setpoint(int16_t setpoint);
			
			void sample_actual();
			
			/*
			 * Returns a formatted string of the voltage / current values.  If actual
			 * is non-zero, return the actual measured values; otherwise, return the
			 * set point values.
			 */
			void to_string(uint8_t index, uint8_t actual, char* buffer, uint8_t length);
			
			/*
			 * Adjust the voltage / current setpoint by the given amount (positive or negative).
			 * The selector will be one of 0 (voltage) or 1 (current).
			 */
			void adjust_setpoint(uint8_t selector, int16_t amount);
	};
}

#endif