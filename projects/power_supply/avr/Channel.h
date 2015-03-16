/*
 * A single power supply channel.  Includes all variables needed for setting DAC, ADC readings, 
 * formatting display, setting values, etc.
 */
#ifndef CHANNEL_H
#define CHANNEL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "lib/twi/twi.h"

#define ADC_CHANNEL_0						0
#define ADC_CHANNEL_1						1
#define ADC_CHANNEL_4						4
#define ADC_CHANNEL_5						5
#define ADC_CHANNEL_6						6
#define ADC_CHANNEL_7						7
#define ADC_CHANNEL_8						8
#define ADC_CHANNEL_9						9
#define ADC_CHANNEL_10						10
#define ADC_CHANNEL_11						11
#define ADC_CHANNEL_12						12
#define ADC_CHANNEL_13						13

//The three I2C addresses for DACs
#define DAC_ADDRESS_0						0x60
#define DAC_ADDRESS_1						0x61
#define DAC_ADDRESS_2						0x62

//Various DAC commands.  See MCP4728 datasheet, page 34
#define DAC_COMMAND_REGISTER				0x40
#define DAC_COMMAND_REGISTER_EEPROM			0x58
#define DAC_COMMAND_ADDRESS					0x60
#define DAC_COMMAND_VREF					0x80
#define DAC_COMMAND_GAIN					0xC0
#define DAC_COMMAND_POWERDOWN				0xA0

//DAC Channels, bits DAC1 and DAC0.  See MCP4728 datasheet
#define DAC_CHANNEL_0						0x00
#define DAC_CHANNEL_1						0x02
#define DAC_CHANNEL_2						0x04
#define DAC_CHANNEL_3						0x06

//TODO Change these to be instance variables set depending on the voltage divider resistor values
#define VOLTAGE_SET_SLOPE			2.5397
#define VOLTAGE_SET_OFFSET			-0.399
#define VOLTAGE_GET_MULTIPLIER	3
#define CURRENT_SET_SLOPE		2.5397
#define CURRENT_SET_OFFSET		-0.399
#define CURRENT_GET_MULTIPLIER	1

#define SELECTOR_VOLTAGE		0
#define SELECTOR_CURRENT		1

#define RUNNING_AVERAGE_COUNT_VOLTAGE	5
#define RUNNING_AVERAGE_COUNT_CURRENT	5

namespace digitalcave {

	class Channel {
		private:
			uint8_t i2c_address;
			uint8_t dac_channel_voltage;
			uint8_t dac_channel_current;
			
			//All voltage values are in millivolts
			int16_t voltage_limit;
			int16_t voltage_setpoint;
			int16_t voltage_actual;
			uint8_t voltage_adc_channel;
			
			//All current values are in milliamps
			int16_t current_limit;
			int16_t current_setpoint;
			int16_t current_actual;
			uint8_t current_adc_channel;
			
			static uint8_t selected_i2c_address_and_reading;	//Used to show menu cursor
			
		public:
			/*
			 * Initializes the channel
			 */
			Channel(uint8_t i2c_address, uint8_t dac_channel_voltage, uint8_t dac_channel_current,
				int16_t voltage_limit, int16_t current_limit,
				uint8_t voltage_adc_channel, uint8_t current_adc_channel);


			int16_t get_voltage_actual();
			int16_t get_voltage_setpoint();
			void set_voltage_setpoint(int16_t setpoint);
			
			int16_t get_current_actual();
			int16_t get_current_setpoint();
			void set_current_setpoint(int16_t setpoint);
			
			void sample_actual();
			
			/*
			 * Adjust the voltage / current setpoint by the given amount (positive or negative).
			 * The selector will be one of 0 (voltage) or 1 (current).
			 */
			void adjust_setpoint(uint8_t selector, int16_t amount);
	};
}

#endif