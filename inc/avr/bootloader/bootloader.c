#include "bootloader.h"

void bootloader_jump(){
#if defined(__AVR_ATmega32U4__)
	//Jump to bootloader.  Copied from http://www.pjrc.com/teensy/jump_to_bootloader.html
	cli();
	
	//Disable USB
	UDCON = _BV(DETACH);
	USBCON = _BV(FRZCLK);
	UCSR1B = 0;
	
	_delay_ms(10);	//Delay at least 5 ms (give it 10 to be safe) to allow host to recognize disconnect
	
	//Disable timers, ADC, and I/O pins
	EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
	TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
	DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
	PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
	
	asm volatile("jmp 0x7E00");
#endif
}