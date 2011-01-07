#include <util/delay.h>
#include "../../lib/serial/serial.h"
#include "../../lib/shift/shift.h"
#include "../../pilot/main.h"

int main(){

	//Init all modules.  We call accel_init last as it forces sei().
	serial_init_b(57600);
	shift_init(&PORT_LED_SHIFT_DATA, PIN_LED_SHIFT_DATA, &PORT_LED_SHIFT_CLOCK, PIN_LED_SHIFT_CLOCK, &PORT_LED_SHIFT_LATCH, PIN_LED_SHIFT_LATCH);
	
	shift_out(0xFF);
	_delay_ms(500);
	shift_out(0x00);
	
	
	//Main program loop
	while (1) {
		char b;
		while (serial_available() && serial_read_c(&b)) {
			shift_out(b);
		}
	}
}

