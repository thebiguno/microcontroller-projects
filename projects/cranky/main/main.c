#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t ign_pins[] = { _BV(PINB2), _BV(PINB3), _BV(PINB1), _BV(PINB0) };
static volatile uint8_t inj_pins[] = { _BV(PIND5), _BV(PIND4), _BV(PIND6), _BV(PIND7) };
volatile uint8_t ign_pin;
volatile uint8_t inj_pin;

//static volatile uint8_t frequency;

/* TODO
 * below a configured RPM (i.e. 300) don't calculate the PWM for the injectors, just use defalt cranking values
 * implement a rev limiter; either retard  the ignition advance, or cut fuel; configurable rev upper limit and restored limit
 * configure if coolant temperature sensor is in use or not (i.e. air cooled engine)
 * configure injector opening time (ms)
 *           injector battery voltage correction (ms/V)
 *           pwm time threshold (ms)
 *           injector pwm period (µs)
 *           molex minifit connectors
 *           coolant temperature guage pwm
 *           clean tacho guage out
 *           determine both spark and injector configuration
 *           organize tuning values into a register array
 *           organize sensor data and computed values into register array
 *           write serial (or usb) code for reading / writing register
 *           use a union for the register
 */

#define PROTOCOL_START  0x7e
#define PROTOCOL_ESCAPE 0x7d

typedef struct {
	volatile uint8_t position;
	volatile uint8_t length;
	volatile uint8_t address;
	volatile uint8_t state;		// bit 0 = escape; bit 1 = framing error;
} protocol_t;

typedef struct {
	// tuning values
	
	// an 8x8 matrix of spark advance values specified in degrees
	// first index is the rpm zone, 0 for slow, 7 for fast
	// second index is the engine load, 0 for low, 7 for high
	// determines how many degrees to advance the spark based on the rpm zone and load zone
	uint8_t ign_advance[8][8];
	// determines how open the injectors should be based on the rpm zone and load zone
	uint8_t inj_duration[8][8];
	// maps the throttle position into load zones
	uint8_t load_zones[8];
	// maps the frequency into rpm zones
	uint8_t rpm_zones[8];

	volatile uint8_t cranking_ticks;	// above this value = cranking; below this value = running normally
	volatile uint8_t max_ign_dwell;		// the maximum dwell in timer0 ticks
	
	// running values
	volatile uint8_t load_zone;			// the current load zone; a value between 0 and 8
	volatile uint8_t rpm_zone;			// the current rpm zone; a value between 0 and 8
	volatile uint8_t cam;				// the current cam tooth; a value between 0 and 3
	volatile uint8_t crank;				// the current crank tooth; a value between 0 and 35
	volatile uint8_t ign;				// the current spark plug; a value between 0 and 3
	volatile uint8_t ign_dwell;			// the spark plug dwell in timer0 ticks
	volatile uint8_t ign_adv_deg;		// the current spark advance in degrees
	volatile uint8_t inj; 				// the current injector; a value between 0 and 3
	volatile uint8_t inj_dc;			// the injector duty cycle in timer2 ticks

	volatile uint8_t crank_sync;	 	// crank sync has been performed; worst case is 10 rotations; 0 means calibrated
	volatile uint8_t crank_ticks;		// the number of timer0 ticks since the last crank tooth; this can be turned into rpm
										// 1000/(t*51.2*36/1000)/0.0166666666667
	volatile uint8_t adc;				// 0 to disable reading values from the adc
	volatile uint8_t adc_tp;			// adc reading of the throttle position sensor
	volatile uint8_t adc_o2;			// adc reading of the exhaust gas oxygen sensor
	volatile uint8_t adc_batt;			// adc reading of the battery voltage
	volatile uint8_t adc_map;			// adc reading of the manifold absolute pressure sensor
	volatile uint8_t adc_mat;			// adc reading of the manifold air temperature
	volatile uint8_t adc_clt;			// adc reading of the coolant temperature
	
	volatile uint8_t read_addr;			// the address to start reading from
	volatile uint8_t read_len;			// the number of bytes to read
} reg_t;

static union reg_u {
	reg_t s;
	uint8_t a[512]; // TODO make this the right size
} u;

static protocol_t input;
static protocol_t output;

// these things are not in the struct since they are never read/write over serial
volatile uint8_t cam_teeth;			// how many crank teeth between cam teeth

/* 
// additional parameters that may become tuning parameters or constants
float inj_ms;
float inj_opening_ms = 1.0;
float inj_battery_correction_ms_V = 0.2;
float inj_pwm_current_limit_pct = 18;
float inj_pwm_time_threshold_ms = 0.7;
float inj_fast_idle_temp_c = -18;

float inj_cranking_ms = 2.0;
float inj_pwm_40c = 11.0;	// -40
float inj_pwm_80c = 2.2;	// +80

//-45, -30, -15, 0, 15, 30, 45, 60, 75
uint8_t enrichment_pct = { 156, 148, 140, 132, 124, 116, 108, 100 };

float tpsdot_threshold_vs = 0.98;
float accel_time_s = 0.2;
float cold_accel_enrichment_ms = 2.0;
float cold_accel_mult_pct = 150;
float decel_fuel_pct = 20;
*/



// the crank position is used to determine injection and spark timing.
// the cam position may be used to determine injection and spark timing for sequential operation
// the battery voltage is used to adjust injector duty cycle and ignition dwell time in a low voltage situation
// the coolant temperature is used to determine the if the fast idle solenoid should be open or closed
// the coolant temperature is also used to adjust the spark advance
// the manifold pressure is used to adjust the spark advance
// the rpm (crank) is used to determine the spark advance
// the throttle possition is used to determine the load


// Low MAP (low engine load) = more spark advance
// High MAP (high engine load) = less spark advance
// Low CLT (cold engine) = more spark advance
// High CLT (warm engine) = less spark advance
// Low RPM = less spark advance
// High RPM = more spark advance

// reading material:
// http://www.vems.hu/wiki/index.php?page=InputTrigger%2FSubaruThirtySixMinusTwoMinusTwoMinusTwo

// teeth
//                SSSSSSS           SSSSSSS           SSSSSSS           SSSSSSS         spark zones (max advance 60 deg)
//    4                 1                 3                 2                 4         TDC
// |                 |                                   |                 |            cam teeth
// |__|||||||||||||__||||||||||||||||__|__|||||||||||||__||||||||||||||||__|__|||       crank teeth
//      2         3     0         1         2         3     0         1                 
// 56789012345678901234501234567890123456789012345678901234501234567890123456789        crank tooth number
//                1                 3                 2                 4               60 deg before TDC

// hardware map
// C6	/rst								c5	adc			exaust o2 input
// d0	rxd		serial						c4	adc			coolant temperature input
// d1	txd		serial						c3	adc			manifold temperature input
// d2	int0	crank hall effect input		c2	adc			manifold pressure input
// d3	int1	cam hall effect input		c1	dout		fast idle solenoid output
// d4			spare dout					c0	dout		fuel pump relay output
// b6	xtal								
// b7	xtal								b5	sck			spare dout
// d5	dout	injector #1 output			b4	miso		ignition #4 output
// d6	dout	ignition #1 output			b3	mosi		injector #4 output
// d7	dout	injector #2 output			b2	dout		ignition #3 output
// b0	dout	ignition #2 output			b1	dout		injector #3 output

// c6	adc		battery voltage input		c7	adc			throttle position input

// The ECM identifies cylinders at TDC and determines ignition timing as follows:
// * There is a crank input signal every 10 deg rotation of the crankshaft
// * The cylinder group (#4,#1 ; #3,#2) is determined by gaps in the input signal
// * cylinders #1 and #2 are at TDC when there is a single gap, #3 and #4 whene there is a double gap
// * TDC #1, #2 is the zero reference point for the teeth on the timing wheel
// * 
int main(void) {
	
	// TODO load these tuning values from EEPROM
	for (uint8_t i = 0; i < 8; i++) {
		for (uint8_t j = 0; j < 8; i++) {
			u.s.ign_advance[i][j] = i * 5 - j;
			u.s.inj_duration[i][j] = i * 10;
		}
		u.s.rpm_zones[7-i] = (i*10) + 5;	// rpm = 382>434>501>592>723>930>1302>2170>8138 (not ideal for real life)
		u.s.load_zones[7-i] = 2 ^ i;		
	}
	
	u.s.crank = 0;
	u.s.cam = 0;
	u.s.crank_ticks = 0;
	u.s.crank_sync = 0;
	cam_teeth = 0;
	
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
	
	// set up pin outputs // TODO, this is now wrong
	DDRB = _BV(PINB0) | _BV(PINB1) | _BV(PINB2) | _BV(PINB3);	// spark plugs
	DDRD = _BV(PIND4) | _BV(PIND5) | _BV(PIND6) | _BV(PIND7);	// injectors

	UCSR0B |= _BV(RXCIE0);		// enable serial rx interrupts
	
	sei();						// enable interrupts

	ADCSRA |= _BV(ADSC);	// start an analog conversion for throttle position

	while(1) {
		// nothing in the main loop is time sensative
		
		for (uint8_t i = 0; i < 8; i++) {
			if (u.s.crank_ticks > u.s.rpm_zones[i]) {
				u.s.rpm_zone = i;
				break;
			}
		}
		for (uint8_t i = 0; i < 8; i++) {
			if (u.s.adc_tp < u.s.load_zones[i]) {
				u.s.load_zone = i;
				break;
			}
		}
		u.s.inj_dc = u.s.inj_duration[u.s.rpm_zone][u.s.load_zone];
		
		// continually poll ADC2 ~ ADC7
		if ((ADCSRA & ADSC) == 0x00) {
			if (ADMUX > 0x7) ADMUX = 0x02;
			else ADMUX++;
			ADCSRA |= _BV(ADSC);
		}
	}
}

// analog ISR
ISR(ADC_vect) {
	switch(ADMUX) {
		case 0x02: u.s.adc_map = ADCH; break;
		case 0x03: u.s.adc_mat = ADCH; break;
		case 0x04: u.s.adc_clt = ADCH; break;
		case 0x05: u.s.adc_o2 = ADCH; break;
		case 0x06: u.s.adc_batt = ADCH; break;
		case 0x07: u.s.adc_tp = ADCH; break;
	}
}

// crank ISR
// the wheel has 36 - 2 - 2 - 2 = 30 teeth
// each tooth represents 10 degrees
ISR(INT0_vect) {
	uint8_t t = TCNT0; // how long 10 (or 30) degrees took
	if (t > (u.s.crank_ticks << 1)) {
		// gap detected add the two missing gap teeth
		u.s.crank = u.s.crank + 2;
		cam_teeth = cam_teeth + 2;
		t = t / 3; // approximate the duration of the single tooth, accounting for the length of the gap
	} else {
		u.s.crank++;
		u.s.cam++;
	}
	
	TCNT0 = 0; // reset timer0 to zero
	
	if (u.s.crank == 13 || u.s.crank == 16 || u.s.crank == 31) {
		// tooth count is wrong, this is normal so just try adjusting
		u.s.crank = u.s.crank + 2;
	} else if (u.s.crank == 12 || u.s.crank == 30) {
		// now 60 degrees before 0/180 degrees, set the timer for the spark advance
		u.s.ign_adv_deg = u.s.ign_advance[u.s.rpm_zone][u.s.load_zone]; // in degrees
		u.s.ign_adv_deg = 10; // TODO remove
		// 60 degrees - advance degrees (1024 prescale -> 8 prescale)
		// t is a /1024 prescaled value and maxes out at about 66 @ 500 rpm
		// OCR1A is a /8 prescaled value
		// multiply by 128 to convert form /1024 to /8
		uint16_t deg = 60 - u.s.ign_adv_deg;
		
		uint16_t comp = deg * t / 10 * 128; // this isn't terribly accurate due to rounding
		OCR1A = comp; // set up timer1 compA for on time
		OCR2A = comp + 2048;
		TCNT1 = 0;	// reset timer1 to zero
		
//		PORTB=0xFF;
	} else if (u.s.crank == 1 && u.s.cam == 2) {
		// #1BDTC; #4 just finished burning, #1 ignition just happened, #3 ignites next, #2 injects next
		u.s.ign = 0;
	} else if (u.s.crank == 1 && u.s.cam == 0) {
		// #2BDTC; #3 just finished burning, #2 ignition just happened, #4 ignites next, #1 injects next
		u.s.ign = 1;
	} else if (u.s.crank == 19 && u.s.cam == 2) {  // in cam signal gap, so 2 not 3
		// #3BTDC; #1 just finished burning, #3 ignition just happened, #2 ignites next, #3 injects next
		u.s.ign = 2;
	} else if (u.s.crank == 19 && u.s.cam == 1) {
		// #4BTDC; #2 just finished burning, #4 ignition just happened, #1 ignites next, #4 injects next
		u.s.ign = 3;
	} else if (u.s.crank == 15 && u.s.cam == 2) {
		u.s.inj = 0;
	} else if (u.s.crank == 15 && u.s.cam == 0) {
		u.s.inj = 1;
	} else if (u.s.crank == 33 && u.s.cam == 2) {
		u.s.inj = 2;
	} else if (u.s.crank == 33 && u.s.cam == 1) {
		u.s.inj = 3;
	} else if (u.s.crank >= 35) {
		u.s.crank = 0;
	}
	ign_pin = ign_pins[u.s.ign];
	inj_pin = inj_pins[u.s.inj];
	
	u.s.crank_ticks = t;
}

// timer 0 overflow
ISR(TIMER0_OVF_vect) {
	// if time 0 overflows it means that the crank is turning slower than 384 rpm
	u.s.crank_ticks = 255;
	TCNT0 = 255;
}

// cam sync ISR
ISR(INT1_vect) {
	// now on either crank tooth 25 or 33
	// RHS camshaft sensor, signals are at BTDC#2, BTDC#4, BTDC#1
	u.s.cam++;
	
	// normally 14 or 16 tooth events between cam events, more means a gap
	if (cam_teeth > 20) {
		// gap detected, now at #2BTDC
		u.s.cam = 0;
	}
	cam_teeth = 0;
}

// ignition spark on
ISR(TIMER1_COMPA_vect) {
	if ((u.s.crank > 12 && u.s.crank < 19) || u.s.crank >= 30 || u.s.crank < 1) {
//		if ((u.s.crank > 11 && u.s.crank < 19) || (u.s.crank > 29 && u.s.crank < 26) || (u.s.crank == 0)) {
		PORTB = ign_pin;
	}
}

// ignition spark off
ISR(TIMER1_COMPB_vect) {
	PORTB = 0x0;
	
	OCR1A = 0xFFFF;
	TCNT1 = 0;
}

// injector pwm frequency
ISR(TIMER2_COMPA_vect) {
	// -30 to +70 degrees around TDC
	if ((u.s.crank > 14 && u.s.crank < 26) || u.s.crank > 33 || u.s.crank < 8) {
		PORTD |= inj_pin;
	}
	TCNT2 = 0;
	OCR2B = (uint16_t) u.s.inj_dc * 165 / 255;	// 165 * 8 clock cycles = 66 us
}

// injector pwm phase
ISR(TIMER2_COMPB_vect) {
	PORTD = 0;
}

void write_pump() {
	if (!(UCSR0A & _BV(UDRE0))) {
		// hardware not ready
		return;
	}
	
	if (output.position == output.length) {
		// nothing to do
		return;
	}
	
	if (output.state == 0x01) {
		UDR0 = 0x20 ^ u.a[output.address++];
		output.state = 0x00;
		return;
	}
	
	uint8_t b = u.a[output.address];
	switch (output.position) {
		case 0:
		UDR0 = PROTOCOL_START;
		break;
		case 1:
		UDR0 = output.length;
		break;
		case 2:
		UDR0 = output.address;
		break;
		default:
		if (b == PROTOCOL_START || b == PROTOCOL_ESCAPE) {
			UDR0 = PROTOCOL_ESCAPE;
			output.state = 0x01;
		} else {
			UDR0 = b;
			output.address++;
		}
		break;
	}
}

ISR(USART_RX_vect) {
	uint8_t b = UDR0;
	
	if (input.state == 0x02 && b == PROTOCOL_START) {
		// recover from any previous error condition
		input.state = 0x00;
		input.position = 0;
	} else {
		return;
	}
	
	if (input.position > 0 && b == PROTOCOL_START) {
		// unexpected start of frame
		input.state = 0x02;
		return;
	}
	
	if (input.position > 0 && b == PROTOCOL_ESCAPE) {
		// unescape the next byte
		input.state = 0x01;
		return;
	}
	
	if (input.state == 0x01) {
		// unescape current byte
		b = 0x20 ^ b;
		input.state = 0x00;
	}
	
	switch (input.position) {
		input.position++;
		case 0:
		break;
		case 1:
		input.length = b;
		break;
		case 2:
		input.address = b;
		break;
		default:
		// TODO add bounds checking once register size is known
		u.a[input.address++ - 3] = b;
		
		if (input.position == input.length) {
			input.position = 0;
		}
		break;
	}
}

