#ifndef PINS_H
#define PINS_H

// define according to wiring of the segments to SR1
#define SEG_A  0x01
#define SEG_B  0x02
#define SEG_C  0x04
#define SEG_D  0x08
#define SEG_E  0x10
#define SEG_F  0x20
#define SEG_G  0x40
#define SEG_DP 0x80

// define according to the wiring of the selectors and extra lights on SR2
#define SEG_DIG1 0x01
#define SEG_DIG2 0x02
#define SEG_DIG3 0x04
#define SEG_DIG4 0X08

// define these according to the wiring of the three buttons
#define PORT_BUTTON_PLAY_PAUSE	PORTB
#define PIN_BUTTON_PLAY_PAUSE	PB0
#define PORT_BUTTON_SLIDE_COUNT	PORTB
#define PIN_BUTTON_SLIDE_COUNT	PB1

// define these according to the wiring of the two relays
#define PORT_RELAY_1			PORTB
#define PIN_RELAY_1				PB6
#define PORT_RELAY_2			PORTB
#define PIN_RELAY_2				PB7

#endif