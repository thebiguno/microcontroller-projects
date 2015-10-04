#include "SoftwareSerialAVR.h"

using namespace digitalcave;

SoftwareSerialAVR::SoftwareSerialAVR(volatile uint8_t* port, uint8_t pin, uint32_t baud){
	this->port = port;
	this->pin = pin;
	this->baud = baud;
	
	*this->port |= _BV(pin);
	*(this->port - 1) |= _BV(pin);
	
#if F_CPU == 16000000
	switch(baud){
		case 19200:
			delayCycles = 3;
			delayCyclesLong = 93;
			break;
		case 38400:
			delayCycles = 5;
			delayCyclesLong = 40;
			break;
		case 57600:
			delayCycles = 2;
			delayCyclesLong = 24;
			break;
		case 115200:
			delayCycles = 13;
			delayCyclesLong = 0;
			break;
		default:
			//9600 baud is default
			delayCycles = 0;
			delayCyclesLong = 199;
	}
#elif F_CPU == 8000000
	switch(baud){
		case 19200:
			delayCycles = 5;
			delayCyclesLong = 40;
			break;
		case 38400:
			delayCycles = 4;
			delayCyclesLong = 14;
			break;
		case 57600:
			delayCycles = 13;
			delayCyclesLong = 0;
			break;
		case 115200:
			//Even with no delays, we can't quite handle 115200 at 8MHz; we get an actual baud of about
			// 107400.  You have been warned...
			delayCycles = 0;
			delayCyclesLong = 0;
			break;
		default:
			//9600 baud is default
			delayCycles = 0;
			delayCyclesLong = 95;
	}
#else
#error Invalid F_CPU value.  Please select 
#endif
}

uint8_t SoftwareSerialAVR::read(uint8_t *c){
	return 0;
}

void SoftwareSerialAVR::delay(){
	if (delayCycles){
		for (uint8_t delay = 0; delay < delayCycles; delay++){
			asm volatile ("nop"::);
		}
	}
	if (delayCyclesLong){
		for (uint8_t delay = 0; delay < delayCyclesLong; delay++){
			asm volatile ("nop"::);
			asm volatile ("nop"::);
			asm volatile ("nop"::);
			asm volatile ("nop"::);
		}
	}
}

uint8_t SoftwareSerialAVR::write(uint8_t data){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		//Start bit
		*port &= ~_BV(pin);
		delay();
		
		for (uint8_t i = 0; i < 8; i++){
			if (data & 0x01){
				*port |= _BV(pin);
			}
			else {
				*port &= ~_BV(pin);
			}
			data = data >> 1;
			delay();
		}
		
		//Stop bit
		*port |= _BV(pin);
		delay();
	}
	
	return 1;
}