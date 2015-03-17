#include "usb.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern State state;

void usb_send_values(uint8_t message, uint8_t channel, uint16_t voltage, uint16_t current){
	uint8_t tx_buffer[6];
	tx_buffer[0] = message;				//Message type
	tx_buffer[1] = channel;				//Channel number
	tx_buffer[2] = (voltage >> 8) & 0xFF;
	tx_buffer[3] = voltage & 0xFF;
	tx_buffer[4] = (current) & 0xFF;
	tx_buffer[5] = current & 0xFF;

	usb_rawhid_send(tx_buffer, 6);
}

void usb_dispatch(){
	uint8_t rx_buffer[16];
	uint8_t length = usb_rawhid_recv(rx_buffer, 0);
	
	if (length > 0){
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
					usb_send_values(MESSAGE_ACTUAL, channel, channels[channel].get_voltage_actual(), channels[channel].get_current_actual());
				}
				break;
			}
			case MESSAGE_ACTUAL_RAW: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					usb_send_values(MESSAGE_ACTUAL_RAW, channel, channels[channel].get_voltage_actual_raw(), channels[channel].get_current_actual_raw());
				}
				break;
			}
			case MESSAGE_SETPOINT: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					usb_send_values(MESSAGE_SETPOINT, channel, channels[channel].get_voltage_setpoint(), channels[channel].get_current_setpoint());
				}
				break;
			}
			case MESSAGE_SETPOINT_RAW: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					usb_send_values(MESSAGE_SETPOINT_RAW, channel, channels[channel].get_voltage_setpoint_raw(), channels[channel].get_current_setpoint_raw());
				}
				break;
			}
			case MESSAGE_CHANGE_SETPOINT: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					channels[channel].set_voltage_setpoint((int16_t) ((rx_buffer[2] << 8) + rx_buffer[3]));
					channels[channel].set_current_setpoint((int16_t) ((rx_buffer[4] << 8) + rx_buffer[5]));
				}
				break;
			}
			case MESSAGE_CHANGE_SETPOINT_RAW: {
				uint8_t channel = rx_buffer[1];
				if (channel < CHANNEL_COUNT){
					channels[channel].set_voltage_setpoint_raw((rx_buffer[2] << 8) + rx_buffer[3]);
					channels[channel].set_current_setpoint_raw((rx_buffer[4] << 8) + rx_buffer[5]);
				}
				break;
			}
		}
	}
}