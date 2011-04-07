/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../../../pins/644.h"
#include "../control.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/serial1/serial1.h"

char temp[32];

control_t control, last_control;
uint16_t buttons, last_buttons;

int main (void){
	//Do setup here

	serial1_init_b(9600);
	serial1_write_s("--reset--\n\r");
	
	control_init();
	serial1_write_s("--init--\n\r");

	//Main program loop
	while (1){

		_delay_ms(10);

		control_update();
		control = control_read_analog();
		buttons = control_button_state();
//		uint16_t buttons_changed = control_button_state_changed();

		if (control.pitch != last_control.pitch 
				|| control.roll != last_control.roll 
				|| control.yaw != last_control.yaw 
				|| control.throttle != last_control.throttle){
			last_control = control;

			sprintf(temp, "%g", control.pitch);
			serial1_write_s(temp);
			serial1_write_s(", ");
			sprintf(temp, "%g", control.roll);
			serial1_write_s(temp);
			serial1_write_s(", ");
			sprintf(temp, "%g", control.yaw);
			serial1_write_s(temp);
			serial1_write_s(", ");
			sprintf(temp, "%g", control.throttle);
			serial1_write_s(temp);
			serial1_write_s("\n\r");
		}
		
		if (buttons != last_buttons) {
            last_buttons = buttons;
            
            if ((buttons & POWER)) serial1_write_s(">");
            if ((buttons & MODE_STABLE)) serial1_write_s("X");
            if ((buttons & MODE_SPORT)) serial1_write_s("O");
            serial1_write_s("\n\r");
		}

	}
}
