/*
 * A single power supply channel.  Includes all variables needed for setting DAC, ADC readings,
 * formatting display, setting values, etc.
 */
#ifndef CHANNEL_H
#define CHANNEL_H

#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include <twi/twi.h>
#include <analog/analog.h>

#define CALIBRATION_COUNT					8

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

typedef struct calibration {
	uint16_t dac;
	uint16_t adc;
	int16_t actual;
} calibration_t;

//The size of one block of calibration (either voltage or current).  Each channel requires two of these.
#define EEPROM_CALIBRATION_OFFSET			0x00
#define EEPROM_CALIBRATION_BLOCK_SIZE		(sizeof(calibration_t) * CALIBRATION_COUNT)
#define EEPROM_STARTUP_OFFSET				(EEPROM_CALIBRATION_BLOCK_SIZE * CHANNEL_COUNT * 2)
#define EEPROM_STARTUP_BLOCK_SIZE			(sizeof(int16_t))

namespace digitalcave {

	class Channel {
		private:
			uint8_t channel_index;

			uint8_t i2c_address;
			uint8_t adc_channel_voltage;
			uint8_t adc_channel_current;
			uint8_t dac_channel_voltage;
			uint8_t dac_channel_current;

			/*
			 * Calibration arrays.
			 */
			calibration_t calibration_voltage[CALIBRATION_COUNT];
			calibration_t calibration_current[CALIBRATION_COUNT];

			int16_t voltage_limit;			//Max (or min, for negative) voltage
			int16_t current_limit;			//Max current

			int16_t voltage_startup;		//Startup voltageto
			int16_t current_startup;		//Startup current

			int16_t voltage_setpoint;		//Desired voltage value (mV)
			uint16_t voltage_setpoint_raw;	//Desired raw DAC value
			uint16_t voltage_actual_raw;	//Actual raw 10 bit ADC value

			uint16_t current_setpoint;		//Desired current value (mA)
			uint16_t current_setpoint_raw;	//Desired raw DAC value
			uint16_t current_actual_raw;	//Actual raw 10 bit ADC value

			void set_dac_raw(uint8_t dac_channel, uint16_t raw_value);

			int16_t get_actual_from_adc(uint16_t adc, calibration_t* calibration_data);
			uint16_t get_dac_from_actual(int16_t actual, calibration_t* calibration_data);

		public:

			/*
			 * Initializes the channel
			 */
			Channel(uint8_t channel_index, uint8_t i2c_address, uint8_t dac_channel_voltage, uint8_t dac_channel_current,
				uint8_t adc_channel_voltage, uint8_t adc_channel_current,
				int16_t voltage_limit, int16_t current_limit);

			/*
			 * Voltage functions
			 */
			int16_t get_voltage_limit();
			int16_t get_voltage_setpoint();
			uint16_t get_voltage_setpoint_raw();
			int16_t get_voltage_actual();
			uint16_t get_voltage_actual_raw();
			void set_voltage_setpoint(int16_t millivolts);
			void set_voltage_setpoint_raw(uint16_t raw_value);
			int16_t get_voltage_startup();
			void set_voltage_startup(int16_t startup);

			/*
			 * Current functions
			 */
			int16_t get_current_limit();
			int16_t get_current_setpoint();
			uint16_t get_current_setpoint_raw();
			int16_t get_current_actual();
			uint16_t get_current_actual_raw();
			void set_current_setpoint(int16_t milliamps);
			void set_current_setpoint_raw(uint16_t raw_value);
			int16_t get_current_startup();
			void set_current_startup(int16_t startup);

			/*
			 * ADC polling
			 */
			void sample_actual();

			/*
			 * Persist / load calibration to / from EEPROM
			 */
			void save_calibration();
			void load_calibration();

			/*
			 * Get / set calibration values
			 */
			calibration_t get_calibration_voltage(uint8_t index);
			calibration_t get_calibration_current(uint8_t index);
			void set_calibration_voltage(uint8_t index, calibration_t calibration);
			void set_calibration_current(uint8_t index, calibration_t calibration);
	};
}

#endif
