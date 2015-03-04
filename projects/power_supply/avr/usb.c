#include "usb.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern State state;

void usb_send_actual_values(){
	static uint8_t timer = 0;

	if (timer++ == 0){
		uint8_t tx_buffer[6];
		for(uint8_t c = 0; c < CHANNEL_COUNT; c++){
			tx_buffer[0] = MESSAGE_SEND_ACTUAL;		//Message type
			tx_buffer[1] = c;						//Channel number
			uint16_t mv = channels[c].get_voltage_actual();	//Sent value is in millivolts
			tx_buffer[2] = (mv >> 8) & 0xFF;
			tx_buffer[3] = mv & 0xFF;
			uint16_t ma = channels[c].get_current_actual();	//Sent value is in milliamps
			tx_buffer[4] = (ma >> 8) & 0xFF;
			tx_buffer[5] = ma & 0xFF;
		
			usb_rawhid_send(tx_buffer, 6);
		}
	}
}

void usb_check_for_updates(){
	uint8_t rx_buffer[16];
	uint8_t length = usb_rawhid_recv(rx_buffer, 0);
	
	if (length > 0){
		switch(rx_buffer[0]){
			case MESSAGE_REQUEST_SETPOINT:	//The computer is asking for current setpoints; send them.
				uint8_t tx_buffer[6];
				for(uint8_t c = 0; c < CHANNEL_COUNT; c++){
					tx_buffer[0] = MESSAGE_SEND_SETPOINT;		//Message type
					tx_buffer[1] = c;						//Channel number
					uint16_t mv = channels[c].get_voltage_setpoint();	//Sent value is in millivolts
					tx_buffer[2] = (mv >> 8) & 0xFF;
					tx_buffer[3] = mv & 0xFF;
					uint16_t ma = channels[c].get_current_setpoint();	//Sent value is in milliamps
					tx_buffer[4] = (ma >> 8) & 0xFF;
					tx_buffer[5] = ma & 0xFF;
	
					usb_rawhid_send(tx_buffer, 6);
				}
				break;
			case MESSAGE_SEND_SETPOINT:	//The computer has just sent new setpoints; load them.
				uint8_t c = rx_buffer[1];
				if (c < CHANNEL_COUNT){
					channels[c].set_voltage_setpoint((rx_buffer[2] << 8) + rx_buffer[3]);
					channels[c].set_current_setpoint((rx_buffer[4] << 8) + rx_buffer[5]);
					
				}
				break;
		}
	}
}