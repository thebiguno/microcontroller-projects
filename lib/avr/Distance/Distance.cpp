#include "Distance.h"

#define TEN_MICROSECONDS (F_CPU / 100000)

using namespace digitalcave;

Distance::Distance(volatile uint8_t *trigger_port, uint8_t trigger_num, volatile uint8_t *data_pin, uint8_t data_num){
	this->trigger_port = trigger_port;
	this->trigger_num = _BV(trigger_num);
	this->data_pin = data_pin;
	this->data_num = _BV(data_num);
}

uint16_t Distance::read(){
	//Bring trigger high for more than 10uS.  20uS seems to work fine.
	*this->trigger_port |= this->trigger_num;
	_delay_us(20);
	*this->trigger_port &= ~this->trigger_num;
	
	//Set up timer2 to run at F_CPU (no prescaler), in normal mode
	TCCR2A = 0x0;
	TCCR2B = _BV(CS20);

	//Determine how long the data pin is high.  Wait until it goes high, then start timing...
	while ((*this->data_pin & this->data_num) == 0x00);
	TCNT2 = 0;
	uint16_t micros = 0;
	
	//Delay until C4 goes low
	while (*this->data_pin & this->data_num){
		if (TCNT2 > TEN_MICROSECONDS) {
			TCNT2 = 0x00;
			micros += 10;
		}
	}
	
	//Turn off the timer
	TCCR2B = 0x00;
	
	//Get any partial time remaining
	micros += ((TCNT2 * (uint16_t) 10) / TEN_MICROSECONDS);
	
	//Calculate distance
	return (uint16_t) (micros / 5.8);
}