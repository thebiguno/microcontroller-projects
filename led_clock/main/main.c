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
	serial_init_b(9600);
	sei();
		
	time_t time;
	
	while(1) {
		// get number of millis since midnight
		uint32_t seconds = timer_get_seconds();
		// convert that into fields
		time_get(seconds, &time);
		// add the millis to the struct
		time.millis = timer_get_millis();

		clock_draw(&time);
		matrix_write_buffer();
		
		if (serial_available()) {
			char action;
			serial_read_c(&action);
			if (action == 'G') { // get
				union u32 conv32;
				union u16 conv16;
				conv32.i = seconds;
				conv16.i = time.millis;
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
				timer_set(conv32.i, conv16.i);
			} else if (action == 'T') { // tune
				union u16 conv16;
				serial_read_c((char*) conv16.a + 0);
				serial_read_c((char*) conv16.a + 1);
				timer_set_tune(conv16.i);
			} else if (action == 'C') {
				union u16 conv16;
				conv16.i = timer_get_tune();
				serial_write_c(conv16.a[0]);
				serial_write_c(conv16.a[1]);
			}
		}
	}
}