#ifndef PINS_H
#define PINS_H

// define according to wiring of the segments to SR1
#define SEG_A  0x20
#define SEG_B  0x80
#define SEG_C  0x10
#define SEG_D  0x04
#define SEG_E  0x02
#define SEG_F  0x08
#define SEG_G  0x40
#define SEG_DP 0x01

// define according to the wiring of the selectors and extra lights on SR2
#define SEG_DIG1 0x02
#define SEG_DIG2 0x04
#define SEG_DIG3 0x08
#define SEG_DIG4 0X10
#define SEG_L1L2 0X40
#define SEG_L3   0X80

// define this according to the wiring of SR1 (and SR3) to AVR
#define PORT_SHIFT_DATA		PORTB
#define PIN_SHIFT_DATA		PINB3
#define PORT_SHIFT_CLOCK	PORTB
#define PIN_SHIFT_CLOCK		PINB5
#define PORT_SHIFT_LATCH	PORTB
#define PIN_SHIFT_LATCH		PINB2

// define these according the wiring of the three buttons
#define PORT_BUTTON_HOUR		PORTD
#define PIN_BUTTON_HOUR			PIND5
#define PORT_BUTTON_MIN			PORTD
#define PIN_BUTTON_MIN			PIND6
#define PORT_BUTTON_MODE		PORTD
#define PIN_BUTTON_MODE			PIND7

#endif