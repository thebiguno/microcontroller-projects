#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lib/analog/analog.h"
#include "lib/timer/timer.h"
#include "lib/serial/serial.h"

//Status LED
#define STATUS_LED_PORT			PORTD
#define STATUS_LED_PIN			PIND5

//Analog pin
#define ANALOG_PIN				0

//XSYNC pin
#define XSYNC_PORT				PORTB
#define XSYNC_PIN				PINB1

//The amount of delta required to register a flash.
#define FLASH_THRESHOLD 		40
//The amount of delta required to re-calibrate ambient
#define AMBIENT_THRESHOLD		-10

//The time after the last command pulse before the 'fire' command happens; in milli seconds.
#define MIN_FIRE_PULSE_DELAY	45
#define MAX_FIRE_PULSE_DELAY	100

//The maximum length of a pulse.  If we read one longer than this, then we will re-adjust
// ambient, as it is likely just than someone has turned the lights on.
#define MAX_PULSE_WIDTH			500
//The maximum count of pulses.  If we exceed this, we will re-adjust ambient, as it is likely
// that we are just sitting on the edge of the flash threshold
#define MAX_PULSE_COUNT			20

//#define DEBUG

uint16_t analog = 0;			//Analog input
uint16_t ambient = 0;			//Ambient light.  Will be adjusted over time.
int16_t flash = 0;				//Delta between ambient and current
	
uint8_t pulse_count = 0;		//How many pulses have happened so far
	
uint64_t status_time = 0;	//Start time of the current pulse
uint64_t pulse_end_time = 0;	//End time of the last pulse
uint64_t time = 0;				//Current time (calculated at start of loop)	

#ifdef DEBUG
char temp[64];
#endif

static inline void fire_flash(){
	XSYNC_PORT |= _BV(XSYNC_PIN);
	_delay_ms(1);
	XSYNC_PORT &= ~_BV(XSYNC_PIN);
	
	//Pulse status 10 times to confirm flash.
	for (uint8_t i = 0; i <= 10; i++){
		STATUS_LED_PORT |= _BV(STATUS_LED_PIN);
		_delay_ms(5);
		STATUS_LED_PORT &= ~_BV(STATUS_LED_PIN);
		_delay_ms(20);
	}
	_delay_ms(100);
}

static inline void read_flash(){
	//Current analog value
	analog_read_a(&analog);
		
	flash = analog - ambient;
}

static inline void read_ambient(){
	uint32_t sum = 0;
	
	for (uint8_t i = 0; i < 16; i++){
		analog_read_a(&ambient);
		_delay_ms(1);
		sum += ambient;
	}
	
	//Divide by 16 (2^4)
	ambient = (sum >> 4);
	
#ifdef DEBUG			
	sprintf(temp, "Adjusted ambient to %d\n\r", ambient);
	serial_write_s(temp);
#endif	
}

static inline void reset(){
	//Reset everything
	pulse_count = 0;
	time = timer_millis();
	read_flash();
}

int main (void){
#ifdef DEBUG
	serial_init_b(9600);
#endif

	//Analog init
	uint8_t pins[1];
	pins[0] = ANALOG_PIN;
	analog_init(pins, 1, ANALOG_INTERNAL);
	
	//Timer init
	timer_init();
		
	//Get initial reading to figure out ambient light; take the average of a number
	// of readings
	read_ambient();

	//Enable XSYNC and LED in output mode
	*(&XSYNC_PORT - 0x1) |= _BV(XSYNC_PIN);
	*(&STATUS_LED_PORT - 0x1) |= _BV(STATUS_LED_PIN);


	//Flash LED five times to confirm we are up and running
	for (uint8_t i = 0; i <= 5; i++){
		STATUS_LED_PORT |= _BV(STATUS_LED_PIN);
		_delay_ms(10);
		STATUS_LED_PORT &= ~_BV(STATUS_LED_PIN);
		_delay_ms(50);
	}

	//Main program loop
	while (1){
		//Current time in milliseconds
		time = timer_millis();

#ifdef DEBUG
		//Print status / heartbeat every second
		if (time - status_time > 1000000){
			sprintf(temp, "Ambient: %d; Analog: %d; Flash: %d\n\r", ambient, analog, flash);
			serial_write_s(temp);
			status_time = time;
		}
#endif		

		read_flash();		
		
		//Start of a new pulse after a delay of more than 40ms but less than 100ms; this is a fire pulse!
		if (pulse_count > 0 
				&& flash > FLASH_THRESHOLD 
				&& time - pulse_end_time > MIN_FIRE_PULSE_DELAY
				&& time - pulse_end_time < MAX_FIRE_PULSE_DELAY){
			fire_flash();

			reset();
		}
		
		//If the last pulse end time was too long ago, reset.
		if (pulse_count > 0
				&& time - pulse_end_time > MAX_FIRE_PULSE_DELAY){
#ifdef DEBUG				
			serial_write_s("Too long since last pulse...\n\r");
#endif
			reset();
		}
		
		//If the analog read value is sufficiently less than ambient, re-read ambient.
		if (flash < AMBIENT_THRESHOLD
			|| pulse_count > MAX_PULSE_COUNT){
#ifdef DEBUG				
			serial_write_s("Flash is negative...\n\r");
#endif
			read_ambient();
		}

		//Start of a new pulse within less than 40ms or more than 100ms (i.e. not a fire pulse)
		if (flash > FLASH_THRESHOLD){
			//Read to the end of the pulse.  If the pulse is lasting longer than MAX_PULSE_WIDTH
			// then we may need to adjust the ambient value.
			while (flash > FLASH_THRESHOLD 
					&& timer_millis() - time < MAX_PULSE_WIDTH){
				read_flash();
			}
#ifdef DEBUG				
			sprintf(temp, "Last pulse ended %8ld us ago\n\r", (uint32_t) (timer_millis() - pulse_end_time));
			serial_write_s(temp);
#endif
			
			pulse_end_time = timer_millis();
			
			//If this 'pulse' was really just ambient, adjust ambient and reset...
			if (timer_millis() - time > MAX_PULSE_WIDTH){
#ifdef DEBUG				
				serial_write_s("Pulse time too long; probably just ambient...\n\r");
#endif
				read_ambient();
				reset();
			}
			//Otherwise, increment pulse count
			else {
				pulse_count++;
#ifdef DEBUG				
				serial_write_s("Incremented pulse count...\n\r");
#endif
			}			
		}
	}
}
