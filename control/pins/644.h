#ifndef PINS_H
#define PINS_H

//The pin definitions for ATMega644P

#define PORT_LCD_DATA			PORTA
#define PIN_LCD_DATA			PINA3
#define PORT_LCD_CLOCK			PORTA
#define PIN_LCD_CLOCK			PINA2
#define PORT_LCD_A0				PORTA
#define PIN_LCD_A0				PINA1
#define PORT_LCD_RESET			PORTA
#define PIN_LCD_RESET			PINA0

#define PORT_PSX_CLOCK			PORTB	// blue
#define PIN_PSX_CLOCK			PINB0
#define PORT_PSX_DATA			PORTB	// brown
#define PIN_PSX_DATA			PINB1
#define PORT_PSX_COMMAND		PORTB	// orange
#define PIN_PSX_COMMAND			PINB2
#define PORT_PSX_ATTENTION		PORTB	// yellow
#define PIN_PSX_ATTENTION		PINB3

#define PORT_LED_ARMED			PORTC
#define PIN_LED_ARMED			PINC2
#define PORT_LED_CRUISE			PORTC
#define PIN_LED_CRUISE			PINC3

#define ADC_BATTERY				7		//ADC 7, pin A7.

//Both pin1 and pin2 must be on the same port
#define PORT_BUZZER				PORTB
#define PIN_BUZZER_1			PINB6
#define PIN_BUZZER_2			PINB7

#endif
