#include "usb.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern State state;

void usb_send_actual_values(){
	//uint8_t tx_buffer[64];
	//TODO Send actual values.  Perhaps hook into a timer to send them every X seconds.
}

void usb_check_for_updates(){
	uint8_t rx_buffer[64];
	uint8_t length = usb_rawhid_recv(rx_buffer, 0);
	
	if (length > 0){
		switch(rx_buffer[0]){
			case MESSAGE_REQUEST_SETPOINT:	//The computer is asking for current setpoints; send them.
				//TODO
				break;
			case MESSAGE_SEND_SETPOINT:	//The computer has just sent new setpoints; load them.
				//TODO
				break;
		}
	}
}