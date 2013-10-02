#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t load_zone;
static volatile uint8_t rpm_zone;
static volatile uint8_t tooth;		// a value between 0 and 35
static volatile uint16_t last_t = 0;	// the time of the last tooth signal
static uint8_t cam;			// a value between 0 and 3
static volatile uint16_t cam_teeth = 0;	// how many teeth between cam signals
static volatile uint8_t pl;	// the current spark plug
static volatile uint8_t in; // the current injector
static volatile uint8_t dc; // the injector duty cycle

static volatile uint8_t plug[] = { _BV(PINB2), _BV(PINB3), _BV(PINB1), _BV(PINB0) };
static volatile uint8_t inj[] = { _BV(PIND5), _BV(PIND4), _BV(PIND6), _BV(PIND7) };
//static volatile uint8_t frequency;

// an 8x8 matrix of spark advance values specified in degrees
// first index is the rpm zone, 0 for slow, 7 for fast
// second index is the engine load, 0 for low, 7 for high
// TODO tune these values
static uint8_t advance[8][8] = {
	// low load     ...     high load
	{  0,  8,  5,  3,  2,  1,  1,  0 },		// slow; < 500 rpm
	{ 14, 12, 10,  8,  8,  6,  6,  5 },
	{ 20, 13, 16, 14, 13, 11, 10,  9 },
	{ 26, 19, 21, 19, 18, 16, 15, 14 },
	{ 32, 21, 26, 24, 23, 21, 20, 18 },
	{ 38, 31, 31, 29, 28, 26, 24, 23 },
	{ 43, 34, 37, 35, 33, 31, 29, 27 },
	{ 46, 44, 42, 40, 38, 36, 34, 32 }		// fast
};

static uint8_t injector[8][8] = {
	// low load    ...    high load
	{ 10, 10, 10, 10, 10, 10, 10, 10 },		// slow; < 500 rpm
	{ 20, 20, 20, 20, 20, 20, 20, 20 },
	{ 30, 30, 30, 30, 30, 30, 30, 30 },
	{ 40, 40, 40, 40, 40, 40, 40, 40 },
	{ 50, 50, 50, 50, 50, 50, 50, 50 },
	{ 60, 60, 60, 60, 60, 60, 60, 60 },
	{ 70, 70, 70, 70, 70, 70, 70, 70 },
	{ 80, 80, 80, 80, 80, 80, 80, 80 },		// fast
};

//In general:
// Low MAP (low engine load) = more spark advance
// High MAP (high engine load) = less spark advance
// Low CLT (cold engine) = more spark advance
// High CLT (warm engine) = less spark advance
// Low RPM = less spark advance
// High RPM = more spark advance

// reading material:
// http://www.vems.hu/wiki/index.php?page=InputTrigger%2FSubaruThirtySixMinusTwoMinusTwoMinusTwo

// teeth
// C                 C                                   C                 C
//    4                 1                 3                 2                 4
// |__|||||||||||||__||||||||||||||||__|__|||||||||||||__||||||||||||||||__|__|||
//                      0        1         2          3 
//                      012345678901234567890123456789012345

// hardware map
// C6	/rst								c5	nc
// d0	rxd									c4	nc
// d1	txd									c3	nc
// d2	int0	crank hall effect input		c2	nc
// d3	int1	cam hall effect input		c1	nc
// d4			injector #1 output			c0	adc			throttle position
// b6	xtal								
// b7	xtal								b5	sck
// d5	dout	injector #2 output			b4	miso
// d6	dout	injector #3 output			b3	mosi/dout	spark #4 output
// d7	dout	injector #4 output			b2	dout		spark #3 output
// b0	dout	spark #1 output				b1	dout		spark #2 output

// The ECM identifies cylinders at TDC and determines ignition timing as follows:
// * There is a crank input signal every 10 deg rotation of the crankshaft
// * The cylinder group (#4,#1 ; #3,#2) is determined by gaps in the input signal
// * cylinders #1 and #2 are at TDC when there is a single gap, #3 and #4 whene there is a double gap
// * TDC #1, #2 is the zero reference point for the teeth on the timing wheel
// * 
int main(void) {
	// timer 0 (8-bit) is used to compute the duration of each crank tooth
	// to determine crank position by detecting gap teeth and to determine RPM
	// prescaler configured so that the timer won't overflow at 500 rpm @ 36 teeth
	// 20MHz clock = 0.05 us per clock cycle
	// 500 rpm = 8.3 Hz = 120 ms / 36 = 3.333 ms/tooth (9.999 ms for missing teeth)
	// 10000 us / 51.2 = 195 < 256 [ / 1024 prescale ]
	// 382 rpm absolute minimum rpm at 1024 prescale
	TCCR0A = 0x00;						// OC0A / OC0B disconnected
	TCCR0B = _BV(CS00) | _BV(CS02);		// clock prescale select = CLK / 1024

	// timer 1 (16-bit) is used to time the spark advance
	// 20MHz clock = 0.05 us per clock cycle
	// 500 rpm = 8.3 Hz = 120 ms / 36 = 3.333 ms/tooth (20 ms covers 60 degrees maximum spark advance)
	// 20000 us / 0.4 =  50,000 < 65,535 [ / 8 prescale ]
	TCCR1A = 0x00;						// OC1A / OC1B disconnected
	TCCR1B = _BV(CS11);					// clock prescale select = CLK /8
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);	// interrupt on timer 1 compare A and B
	
	// timer 2 (8-bit) is used to drive the injector solenoids (high-z)
	// 20MHz clock = 0.05 us per clock cycle
	// 256 * 8 = 2048 * 0.05 us = 102 us frequency > 66
//	TCCR2A = 0x00;						// OC2A / OC2B disconnected
//	TCCR2B = _BV(CS21);					// clock precale select = CLK / 8
//	TIMSK2 = _BV(OCIE2A) | _BV(OCIE2B); // interrupt on timer 2 compare A and B
//	OCR2A = 165; // 165 * 8 clock cycles = 66 us
	
	// set up analog
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // enable, enable interrupt, prescale /128
	ADMUX |= _BV(ADLAR); // use AREF, left adjust result, read ADC0/PC0

	// set up pin interrupts
	EICRA = _BV(ISC00) | _BV(ISC01) | _BV(ISC10) | _BV(ISC11); // INT0 and INT1 interrupt on rising edge
	EIMSK = _BV(INT0) | _BV(INT1); // enable interrupts on INT0 (crank) and INT1 (cam sync)
	
	// set up pin outputs
	DDRB = _BV(PINB0) | _BV(PINB1) | _BV(PINB2) | _BV(PINB3);	// spark plugs
	DDRD = _BV(PIND4) | _BV(PIND5) | _BV(PIND6) | _BV(PIND7);	// injectors

	sei();

	ADCSRA |= _BV(ADSC);	// start an analog conversion for throttle position

	while(1) {
		// do nothing, all code is in an ISR
	}
}

// analog ISR
ISR(ADC_vect) {
	// throttle position
	if (ADCH < 32) {
		load_zone = 0;
	} else if (ADCH < 64) {
		load_zone = 1;
	} else if (ADCH < 96) {
		load_zone = 2;
	} else if (ADCH < 128) {
		load_zone = 3;
	} else if (ADCH < 160) {
		load_zone = 4;
	} else if (ADCH < 192) {
		load_zone = 5;
	} else if (ADCH < 224) {
		load_zone = 6;
	} else {
		load_zone = 7;
	}
	
	dc = injector[rpm_zone][load_zone];
}

// crank ISR
// the wheel has 36 - 2 - 2 - 2 = 30 teeth
// each tooth represents 10 degrees
ISR(INT0_vect) {
	uint8_t t = TCNT0; // how long 10 (or 30) degrees took
	if (t > (last_t << 1)) {
		// gap detected add the two missing gap teeth
		tooth = tooth + 2;
		cam_teeth = cam_teeth + 2;
		t = t / 3; // approximate the duration of the single tooth, accounting for the length of the gap
	} else {
		tooth++;
		cam_teeth++;
	}
	
	// TODO change this to a running average instead a point calculation
	last_t = t;
	TCNT0 = 0; // reset timer0 to zero
	
	if (tooth > 35) {
//		PORTB = 0xFF;
		tooth = 0;
		
		// determine rpm zone once per revolution
		// TODO tune these rpm zone values
		if (t > 128) { // ~500 rpm and slower
			rpm_zone = 0;
		} else if (t > 64) {
			rpm_zone = 1;
		} else if (t > 32) {
			rpm_zone = 2;
		} else if (t > 16) {
			rpm_zone = 3;
		} else if (t > 8) {
			rpm_zone = 4;
		} else if (t > 4) {
			rpm_zone = 5;
		} else if (t > 2) {
			rpm_zone = 6;
		} else {
			rpm_zone = 7;
		}
		
		// start ADC again -- read throttle position once per revolution
		//ADCSRA |= _BV(ADSC);
	} else if (tooth == 13 || tooth == 14 || tooth == 16 || tooth == 17 || tooth == 31 || tooth == 32) {
		// tooth count is wrong, this is normal so just try adjusting
		tooth++;
		if (tooth == 13 || tooth == 16 || tooth == 31) {
			tooth++;
		}
		
	} else if (tooth == 30 || tooth == 12) {
		// now 60 degrees before 0/180 degrees, set the timer for the spark advance
		uint8_t adv = advance[rpm_zone][load_zone]; // in degrees
		//adv = 10;
		// 60 degrees - advance degrees (1024 prescale -> 8 prescale)
		// t is a /1024 prescaled value and maxes out at about 66 @ 500 rpm
		// OCR1A is a /8 prescaled value
		// multiply by 128 to convert form /1024 to /8
		uint16_t deg = 60 - adv;
		
		TCNT1 = 0;	// reset timer1 to zero
		// TODO this isn't terribly accurate due to rounding
		uint16_t comp = deg * t / 10 * 128;
		OCR1A = comp; // set up compare value for timer1
		OCR1B = comp + 2048; // TODO  a guess
	}

	if (tooth == 1 && cam == 2) {
		// #1BDTC; #4 just finished burning, #1 ignition just happened, #3 ignites next, #2 injects next
		pl = plug[0];
	} else if (tooth == 1 && cam == 0) {
		// #2BDTC; #3 just finished burning, #2 ignition just happened, #4 ignites next, #1 injects next
		pl = plug[1];
	} else if (tooth == 19 && cam == 2) {  // in cam signal gap, so 2 not 3
		// #3BTDC; #1 just finished burning, #3 ignition just happened, #2 ignites next, #3 injects next
		pl = plug[2];
	} else if (tooth == 19 && cam == 1) {
		// #4BTDC; #2 just finished burning, #4 ignition just happened, #1 ignites next, #4 injects next
		pl = plug[3];
	} else if (tooth == 15 && cam == 2) {
		in = inj[0];
	} else if (tooth == 15 && cam == 0) {
		in = inj[1];
	} else if (tooth == 33 && cam == 2) {
		in = inj[2];
	} else if (tooth == 33 && cam == 1) {
		in = inj[3];
	}
}

// cam sync ISR
ISR(INT1_vect) {
	// now on either tooth 25 or 33
	// RHS camshaft sensor, signals are at BTDC#2, BTDC#4, BTDC#1
	cam++;
	
	// normally 14 or 16 tooth events between cam events, more means a gap
	if (cam_teeth > 20) {
		// gap detected, now at #2BTDC
		cam = 0;
		//tooth = 33;
	}
	
	cam_teeth = 0;
}

// ignition spark event
ISR(TIMER1_COMPA_vect) {
	if ((tooth > 12 && tooth < 19) || tooth >= 30 || tooth < 1) {
		PORTB = pl;
	}
}
ISR(TIMER1_COMPB_vect) {
	PORTB = 0x00;
}

// injector pwm frequency
ISR(TIMER2_COMPA_vect) {
	// -30 to +70 degrees around TDC
	if ((tooth > 14 && tooth < 26) || tooth > 33 || tooth < 8) {
		PORTD |= in;
	}
	TCNT2 = 0;
	OCR2B = (uint16_t) dc * 165 / 255;	// 165 * 8 clock cycles = 66 us
}

// injector pwm phase
ISR(TIMER2_COMPB_vect) {
	PORTD = 0;
}

