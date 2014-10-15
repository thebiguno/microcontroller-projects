#include "distance.h"

volatile uint8_t interval_do_distance_reading = 0x00;		//Mailbox value, set in timer2 ISR and read in delay

#if DISTANCE_SENSOR == 1

#define TEN_MICROSECONDS (F_CPU / 100000)

static volatile int16_t distance;

static volatile uint16_t micros = 0;						//Microsecond counter
static volatile uint8_t mutex = 0x00;						//Only start a new reading if we are not already in one.

void distance_init(){
	DDRA |= _BV(PORTA0);
}

void distance_measure(){
	if (mutex) return;
	mutex = 0x01;

	//Enable pin change interrupt 1 (start and stop timer as the pin changes state)
	PCICR |= _BV(PCIE0);
	PCMSK0 |= _BV(PCINT1);
	
	//Set up output compare register to fire every 10 microseconds
	OCR0A = TEN_MICROSECONDS;
	
	//Bring trigger high for more than 10uS.  20uS seems to work fine.
	PORTA |= _BV(PORTA0);
	_delay_us(20);
	PORTA &= ~_BV(PORTA0);
	
	//That's it!  The pin change ISR will then handle starting the timer, stopping the timer, etc.
}

uint16_t distance_read(){
	return distance;
}

ISR(TIMER0_COMPA_vect){
	sei();
	TCNT0 = 0x00;
	micros += 10;
}

void doSendDebug(char* message, uint8_t length);

ISR(PCINT0_vect){
	sei();

	//Rising value; we are starting a distance sample.
	if (PINA & _BV(PORTA1)){
		//Set up timer0 to run at F_CPU (no prescaler), in normal mode
		TCNT0 = 0;
		TCCR0B = _BV(CS00);
		TCCR0A = 0x0;
		TIMSK0 |= _BV(OCIE0A);

		micros = 0;
	}
	//Falling value; we are finishing a distance sample.
	else {
		//Get any partial time remaining
		micros += ((TCNT0 * (uint16_t) 10) / TEN_MICROSECONDS);

		//Turn off the timer
		TCCR0B = 0x00;
		
		//Disable pin change interrupts
		PCICR &= ~_BV(PCIE0);
		PCMSK0 &= ~_BV(PCINT1);
		
		//Calculate distance; 5.8 is a magic number calculated from the speed of sound and found in the datasheet.
		distance = (uint16_t) (micros / 5.8);
		
		mutex = 0x00;
	}
}

#else

void distance_init(){};

void distance_measure(){};

uint16_t distance_read(){ return 0xFFFF; }

#endif