/*
 * A single power supply channel.  Includes all variables needed for setting DAC, ADC readings, 
 * formatting display, setting values, etc.
 */

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

namespace digitalcave {

	class Channel {
		private:
			uint8_t i2c_address;
			
			
			double voltage_max;
			double voltage_min;
			double voltage_setpoint;
			double voltage_actual;
			uint8_t voltage_adc_channel;
			
			double current_max;
			double current_min;
			double current_setpoint;
			double current_actual;
			uint8_t current_adc_channel;
			
			static uint8_t selected_i2c_address_and_reading;	//Used to show menu cursor
			
		public:
			/*
			 * Initializes the channel
			 */
			Channel(uint8_t i2c_address, 
					double voltage_max, double voltage_min, uint8_t voltage_adc_channel, 
					double current_max, double current_min, uint8_t current_adc_channel);

			double get_voltage_setpoint();
			void set_voltage_setpoint(double setpoint);
			
			double get_current_setpoint();
			void set_current_setpoint(double setpoint);
			
			void sample_actual();
			
			/*
			 * Returns a formatted string of the voltage / current values.  If actual
			 * is non-zero, return the actual measured values; otherwise, return the
			 * set point values.
			 */
			void to_string(uint8_t actual, char* buffer, uint8_t length);
			
			/*
			 * Adjust the voltage / current setpoint by the given amount (positive or negative).
			 * The selector will be one of 0 (voltage) or 1 (current).
			 */
			void adjust_setpoint(uint8_t selector, double amount);
	};
}