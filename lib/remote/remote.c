/*
NEC IR transmission protocol:
http://techdocs.altium.com/display/ADRR/NEC+Infrared+Transmission+Protocol
														@ 8 Mhz/256
9 ms leading pulse										281 (overflow)
4.5 ms leading space									140
8 bit device address							
8 bit logical inverse of the device address
8 bit command
8 bit logical inverse of the command
562.5 us trailing pulse									17
2250 us: 562.5 us high, 1687.5 us low = logical 1		70 = 17 + 53 
1125 us: 562.5 us high, 562.5 us low = logical 0		35 = 17 + 17

9 ms leading pulse										281
2.25 ms leading pulse									70
562.5 us trailing pulse									17

Apple aluminum remote codes: (Apple ID EE 87, Command, Remote ID)
https://en.wikipedia.org/wiki/Apple_Remote
	Up key:     EE 87 0B/0A 59
	Down key:   EE 87 0D/0C 59
	Left key:   EE 87 08/09 59
	Right key:  EE 87 07/06 59
	Center key: EE 87 5D/5C 59
	Menu key:   EE 87 02/03 59
	Play key:   EE 87 5E/5F 59
*/

volatile uint8_t _state;	// 0 = idle; 1 = leading pulse; 2 = message;
volatile uint8_t _byte;
volatile uint8_t _bit;
volatile uint8_t _packet[4];
volatile uint8_t _command;

void remote_init() {
	// TODO dynamic prescale based on F_CPU; current values appropriate only for 8MHz
	
	#if defined(__AVR_ATtiny13__)
	TCCR0A = 0x0; 						// normal mode
	TCCR0B |= _BV(CS02);				// F_CPU / 256 prescaler
	MCUCR |= _BV(ISC00);				// logical change generates interrupt
	GIMSK |= _BV(INT0);					// enable external interrupts on int0
	#elif defined(__AVR_ATmega168__)   || \
		defined(__AVR_ATmega328__)     || \
		defined(__AVR_ATmega328P__)    || \
		defined(__AVR_ATmega324P__)    || \
		defined(__AVR_ATmega644__)     || \
		defined(__AVR_ATmega644P__)    || \
		defined(__AVR_ATmega644PA__)   || \
		defined(__AVR_ATmega1284P__)
	TCCR0A = 0x0; 						// normal mode
	TCCR0B |= _BV(CS02);				// F_CPU / 256 prescaler
	EICRA |= _BV(ISC00);				// logical change generates interrupt
	EIMSK |= _BV(INT0);					// enable external interrupts on int0
	#endif
	
	sei();
}

uint8_t remote_state() {
	return _state;
}

uint8_t remote_command() {
	uint8_t result = _command;
	_command = 0;
	return result;
}

ISR(TIM0_OVF_vect) {
	if (_state == 0 && (PORTB & _BV(PB1))) {
		_state = 1;
	}
}
EMPTY_INTERRUPT(TIM0_COMPA_vect)
}
EMPTY_INTERRUPT(TIM0_COMPB_vect)

ISR(INT0_vect) {
	if (PORTB & _BV(PB1)) {
		if (_state == 1) {
			if (TCNT > 105) {
				_state = 2;
				_byte = 0;
				_bit = 0;
				_packet[0] = 0;
				_packet[1] = 0;
				_packet[2] = 0;
				_packet[3] = 0;
			} else {
				_state = 0;
				_command = _packet[2];
			}
		} else if (_state == 2) {
			if (TCNT > 35) { // TODO define this constant based on F_CPU
				_packet[_byte] &= _BV(_bit++);
			}
			if (bit == 0) {
				byte++;
				
				if (byte == 5) {
					if (_packet[0] == 0xEE && _packet[1] == 0x87) { // TODO, check _packet[3] for pairing
						_command = _packet[2];
					}
					_state = 0;
				}
			}
		}
	}
	TCNT0 = 0;
}
