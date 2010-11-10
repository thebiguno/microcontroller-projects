#include <avr/io.h>

#include "main.h"

int main (void){
    uint8_t power = 0;
    uint8_t flags = 0;
    uint8_t power_debounce = 0;
    
	//Init communication hardware
	comm_init();

	//Init control interface
	control_init();
	
	//Main program loop
	while (1){
		//Read controls
        control_t control = get_control();
        uint16_t buttons = get_buttons();
        
        if (power_debounce == 0 && buttons & POWER_ON) {
            power_debounce = 1;
            power ^= 1; // toggle power state
        } else if (!(buttons & POWER_ON)) {
            power_debounce = 0;
        }
        flags = power == 1 ? 0x01 : 0x00;
        
        //Send control data
        comm_tx_ctrl(control, flags);
    }
}

