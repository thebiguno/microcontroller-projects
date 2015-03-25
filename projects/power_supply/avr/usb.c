#include "usb.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern State state;

union udouble {
	double d;
	uint8_t u[sizeof(double)];
};

void convert_double_to_bytes(double value, uint8_t *buffer, uint8_t offset){
	union udouble converter;
	converter.d = value;
	for (uint8_t i = 0; i < sizeof(double); i++) {
		buffer[i + offset] = converter.u[i];
	}
}
double convert_bytes_to_double(uint8_t *buffer, uint8_t offset){
	union udouble converter;
	for (uint8_t i = 0; i < sizeof(double); i++) {
		converter.u[i] = buffer[i + offset];
	}
	return converter.d;
}

void usb_send_measurement(uint8_t message, uint8_t channel, uint16_t voltage, uint16_t current){
	uint8_t tx_buffer[6];
	tx_buffer[0] = message;				//Message type
	tx_buffer[1] = channel;				//Channel number
	tx_buffer[2] = (voltage >> 8) & 0xFF;
	tx_buffer[3] = voltage & 0xFF;
	tx_buffer[4] = (current >> 8) & 0xFF;
	tx_buffer[5] = current & 0xFF;

	usb_rawhid_send(tx_buffer, 6);
}

void usb_send_calibration(uint8_t message, uint8_t channel, uint8_t target, double calibration_value){
	uint8_t tx_buffer[7];
	tx_buffer[0] = message;				//Message type
	tx_buffer[1] = channel;				//Channel number
	tx_buffer[2] = target;
	convert_double_to_bytes(calibration_value, tx_buffer, 3);

	usb_rawhid_send(tx_buffer, 7);
}

void usb_send_dac_program(uint8_t dac_number){
	uint8_t tx_buffer[2];
	tx_buffer[0] = MESSAGE_CONFIGURE_DAC_ADDRESS;				//Message type
	tx_buffer[1] = dac_number;				//Channel number

	usb_rawhid_send(tx_buffer, 2);
}

void usb_dispatch(){
	uint8_t rx_buffer[6];
	uint8_t length = usb_rawhid_recv(rx_buffer, 0);
	
	if (length >= 2){
		switch(rx_buffer[0]){
			case MESSAGE_CHANNELS: {
				
				uint8_t tx_buffer[2];
				tx_buffer[0] = MESSAGE_CHANNELS;
				tx_buffer[1] = CHANNEL_COUNT;
				usb_rawhid_send(tx_buffer, 2);
				break;
			}
			case MESSAGE_ACTUAL: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					usb_send_measurement(MESSAGE_ACTUAL, channel, channels[channel].get_voltage_actual(), channels[channel].get_current_actual());
				}
				break;
			}
			case MESSAGE_ACTUAL_RAW: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					usb_send_measurement(MESSAGE_ACTUAL_RAW, channel, channels[channel].get_voltage_actual_raw(), channels[channel].get_current_actual_raw());
				}
				break;
			}
			case MESSAGE_SETPOINT: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					usb_send_measurement(MESSAGE_SETPOINT, channel, channels[channel].get_voltage_setpoint(), channels[channel].get_current_setpoint());
				}
				break;
			}
			case MESSAGE_SETPOINT_RAW: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					usb_send_measurement(MESSAGE_SETPOINT_RAW, channel, channels[channel].get_voltage_setpoint_raw(), channels[channel].get_current_setpoint_raw());
				}
				break;
			}
			case MESSAGE_CHANGE_SETPOINT: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					channels[channel].set_voltage_setpoint((int16_t) ((rx_buffer[2] << 8) + rx_buffer[3]));
					channels[channel].set_current_setpoint((int16_t) ((rx_buffer[4] << 8) + rx_buffer[5]));
					usb_send_measurement(MESSAGE_CHANGE_SETPOINT, channel, channels[channel].get_voltage_setpoint(), channels[channel].get_current_setpoint());
				}
				break;
			}
			case MESSAGE_CHANGE_SETPOINT_RAW: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					channels[channel].set_voltage_setpoint_raw((rx_buffer[2] << 8) + rx_buffer[3]);
					channels[channel].set_current_setpoint_raw((rx_buffer[4] << 8) + rx_buffer[5]);
					usb_send_measurement(MESSAGE_CHANGE_SETPOINT_RAW, channel, channels[channel].get_voltage_setpoint_raw(), channels[channel].get_current_setpoint_raw());
				}
				break;
			}
			case MESSAGE_GET_CALIBRATION: {
				uint8_t channel = rx_buffer[1];
				uint8_t target = rx_buffer[2];
				double value = 0;
				switch(target){
					case TARGET_VOLTAGE_ACTUAL_SLOPE: value = channels[channel].voltage_actual_slope; break;
					case TARGET_VOLTAGE_ACTUAL_OFFSET: value = channels[channel].voltage_actual_offset; break;
					case TARGET_VOLTAGE_SETPOINT_SLOPE: value = channels[channel].voltage_setpoint_slope; break;
					case TARGET_VOLTAGE_SETPOINT_OFFSET: value = channels[channel].voltage_setpoint_offset; break;
					case TARGET_CURRENT_ACTUAL_SLOPE: value = channels[channel].current_actual_slope; break;
					case TARGET_CURRENT_ACTUAL_OFFSET: value = channels[channel].current_actual_offset; break;
					case TARGET_CURRENT_SETPOINT_SLOPE: value = channels[channel].current_setpoint_slope; break;
					case TARGET_CURRENT_SETPOINT_OFFSET: value = channels[channel].current_setpoint_offset; break;
				}
				
				usb_send_calibration(MESSAGE_SET_CALIBRATION, channel, target, value);
				break;
			}
			case MESSAGE_SET_CALIBRATION: {
				uint8_t channel = rx_buffer[1];
				uint8_t target = rx_buffer[2];
				double value = convert_bytes_to_double(rx_buffer, 3);
				switch(target){
					case TARGET_VOLTAGE_ACTUAL_SLOPE: channels[channel].voltage_actual_slope = value; break;
					case TARGET_VOLTAGE_ACTUAL_OFFSET: channels[channel].voltage_actual_offset = value; break;
					case TARGET_VOLTAGE_SETPOINT_SLOPE: channels[channel].voltage_setpoint_slope = value; break;
					case TARGET_VOLTAGE_SETPOINT_OFFSET: channels[channel].voltage_setpoint_offset = value; break;
					case TARGET_CURRENT_ACTUAL_SLOPE: channels[channel].current_actual_slope = value; break;
					case TARGET_CURRENT_ACTUAL_OFFSET: channels[channel].current_actual_offset = value; break;
					case TARGET_CURRENT_SETPOINT_SLOPE: channels[channel].current_setpoint_slope = value; break;
					case TARGET_CURRENT_SETPOINT_OFFSET: channels[channel].current_setpoint_offset = value; break;
				}
				channels[channel].save_calibration();
				
				usb_send_calibration(MESSAGE_SET_CALIBRATION, channel, target, value);
				break;
			}
			case MESSAGE_CONFIGURE_DAC_ADDRESS: {
				uint8_t old_dac_number = rx_buffer[1];
				uint8_t new_dac_number = rx_buffer[2];
				if (old_dac_number > 2 || new_dac_number > 2){
					usb_send_dac_program(0xFF);
					break;
				}
				
				PORTB |= _BV(PORTB0);
				
				uint8_t message[3];
				message[0] = DAC_COMMAND_ADDRESS | old_dac_number << 2 | 1;		//Command 0x60 + current address (left shifted 2) + 1
				message[1] = DAC_COMMAND_ADDRESS | new_dac_number << 2 | 2;		//Command 0x60 + new address (left shifted 2) + 2
				message[2] = DAC_COMMAND_ADDRESS | new_dac_number << 2 | 3;		//Command 0x60 + new address (left shifted 2) + 3
				PORTB &= ~_BV(PORTB0);
				twi_write_to(DAC_ADDRESS_0 + old_dac_number, message, 2, TWI_NO_BLOCK, TWI_STOP);
				
			}
		}
	}
}