#include "bootloader.h"
#include <avr/wdt.h>

void bootloader_jump(uint8_t bootloader){
	//Jump to bootloader.  Copied from http://www.pjrc.com/teensy/jump_to_bootloader.html
	cli();

	//Disable USB
	UDCON = _BV(DETACH);
	USBCON = _BV(FRZCLK);
	UCSR1B = 0;

	_delay_ms(100);	//Delay at least 5 ms (give it 100 to be safe) to allow host to recognize disconnect

	//Disable timers, ADC, and I/O pins
	EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
	TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
	DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
	PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;

	if (bootloader == BOOTLOADER_ATMEL){
		//Atmel has a 4096 byte bootloader
		asm volatile("jmp 0x7000");
	}
	else if (bootloader == BOOTLOADER_LUFA){
		//LUFA bootloader is 512 bytes
		asm volatile("jmp 0x7E00");
	}
	//TODO There are potentially two other sizes of bootloaders possible - 1024 and 2048 bytes.  If we find other bootloaders that fit in these
	// sizes, we can add 0x7C00 and 0x7800 jump addresses respectively.
}
