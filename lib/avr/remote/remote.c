#include "remote.h"
#include <avr/interrupt.h>

/*
NEC IR transmission protocol:
http://techdocs.altium.com/display/ADRR/NEC+Infrared+Transmission+Protocol
														@ 8 MHz/1024	@ 16 MHz/1024	@ 20MHz/1024
9 ms leading pulse										70 				140				176
4.5 ms leading space									35				70				88
8 bit device address							
8 bit logical inverse of the device address
8 bit command
8 bit logical inverse of the command
562.5 us trailing pulse									4				8				11
2250 us: 562.5 us high, 1687.5 us low = logical 1		17 = 4 + 13		43 = 17 + 26	43 = 11 + 33
1125 us: 562.5 us high, 562.5 us low = logical 0		8 = 4 + 4		16 = 8 + 8		22 = 11 + 11

9 ms leading pulse										70				140				176
2.25 ms leading pulse									17				35				44
562.5 us trailing pulse									4				8				11

Apple aluminum remote codes: (Apple ID EE 87, Command, Remote ID)
https://en.wikipedia.org/wiki/Apple_Remote
	Up key:     EE 87 0B/0A 59
	Down key:   EE 87 0D/0C 59
	Left key:   EE 87 08/09 59
	Right key:  EE 87 07/06 59
	Center key: EE 87 5D/5C 59
	Menu key:   EE 87 02/03 59
	Play key:   EE 87 5E/5F 59
	
TODO implement additional remote protocols all with the same interface (i.e. remote_sony.c, remote_rca.c, etc...)
http://www.sbprojects.com/knowledge/ir/nec.php
*/

// leading pulse must be at least 4.5 ms
#define LEADING_PULSE F_CPU / 1024 / 222
// leading space must be at least 3.4 ms for a new command, otherwise it's a repeat command
#define LEADING_SPACE F_CPU / 1024 / 296
// bit space must be at least 1126 us for logical 1, otherwise it's logical 0
#define BIT_SPACE F_CPU / 1024 / 888

volatile uint8_t _state;	// 0 = idle; 1 = leading pulse; 2 = message;
volatile uint8_t _byte_pos;
volatile uint8_t _bit_pos;
volatile uint8_t _byte;
volatile uint8_t _command;

void remote_init() {
	DDRD &= ~_BV(PD2);  // set int0 as input
	
	TCCR0A = 0x0; 						// normal mode
	TCCR0B = _BV(CS02) | _BV(CS00);		// F_CPU / 1024 prescaler

	#if defined(__AVR_ATtiny13__)      || \
		defined(__AVR_ATtiny85__)
	MCUCR |= _BV(ISC00);				// logical change generates interrupt
	GIMSK |= _BV(INT0);					// enable external interrupts on int0
	#elif defined(__AVR_ATmega48__)    || \
		defined(__AVR_ATmega48P__)     || \
		defined(__AVR_ATmega168__)     || \
		defined(__AVR_ATmega328__)     || \
		defined(__AVR_ATmega328P__)    || \
		defined(__AVR_ATmega324P__)    || \
		defined(__AVR_ATmega644__)     || \
		defined(__AVR_ATmega644P__)    || \
		defined(__AVR_ATmega644PA__)   || \
		defined(__AVR_ATmega1284P__)
	EICRA = _BV(ISC00);					// logical change generates interrupt
	EIMSK |= _BV(INT0);					// enable external interrupts on int0
	#endif
	
	sei();
}

uint8_t remote_state() {
	return _state;
}

uint8_t remote_get() {
	uint8_t result = _command;
	_command = 0;
	return result;
}

ISR(INT0_vect) {
	if (PIND & _BV(PD2)) {
		// receiver high; protocol low
		if (_state == 0) {
			if (TCNT0 > LEADING_PULSE) {
				_state = 1;
			}
		}
	} else {
		// receiver low; protocol high
		if (_state == 1) {
			if (TCNT0 > LEADING_SPACE) {
				_state = 2;
				_byte_pos = 0;
				_bit_pos = 0;
				_byte = 0;
			} else {
				_state = 0;
				// TODO implement repeats
			}
		} else if (_state == 2) {
			if (TCNT0 > BIT_SPACE) {
				_byte |= _BV(_bit_pos);
			}
			if (_bit_pos++ == 7) {
				switch (_byte_pos++) {
					case 0:
					if (_byte != 0xee) _state = 0;
					break;
					case 1:
					if (_byte != 0x87) _state = 0;
					break;
					case 2:
					_command = _byte & 0xfe;
					break;
					case 3:
					// TODO check pairing
					_state = 0;
					break;
				}
			}
		}
	}
	TCNT0 = 1;
}
