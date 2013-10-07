#include "timer/timer.h"
#include "clock/clock.h"

#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"

#include "lib/serial/serial.h"
#include "lib/time/time.h"

union u32 {
	uint32_t i;
    char a[sizeof(uint32_t)];
};
union u16 {
	uint16_t i;
	char a[sizeof(uint16_t)];
};

int main() {
	matrix_init();
	matrix_set_mode(0x01);
	timer_init();
	serial_init(9600, 8, 0, 1);
 	sei();
		
	time_t time;
	
	while(1) {
		uint16_t millis = timer_get_millis();
 		uint32_t seconds = timer_get_seconds();

		// get number of millis since midnight
		time_get(seconds, &time);
		uint32_t ms = ((uint32_t) time.second + (uint32_t) time.minute * 60 + (uint32_t) time.hour * 60 * 60) * 1000 + millis;

		clock_draw(&time, ms);
		matrix_write_buffer();
		
		if (serial_available()) {
			char action;
			serial_read_c(&action);
			if (action == 'G') { // get
				union u32 conv32;
				union u16 conv16;
				conv32.i = seconds;
				conv16.i = millis;
				for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
					serial_write_c(conv32.a[i]);
			    }
				for (uint8_t i = 0; i < sizeof(uint16_t); i++) {
					serial_write_c(conv16.a[i]);
			    }
			} else if (action == 'S') { // set
				union u32 conv32;
				union u16 conv16;
				for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
					serial_read_c(conv32.a + i);
				}
				for (uint8_t i = 0; i < sizeof(uint16_t); i++) {
					serial_read_c(conv16.a + i);
				}
				timer_set_seconds(conv32.i);
				timer_set_millis(conv16.i);
			} else if (action == 'T') { // tune
				int8_t t = 0;
				serial_read_c((char*) &t);
				timer_set_tune(t);
			} else if (action == 'C') {
				serial_write_c((char) timer_get_tune());
			}
		}
	}
}