#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lib/analog/analog.h"
#include "lib/Button/Buttons.h"
#include "lib/Hd44780/Display.h"
#include "lib/Hd44780/Hd44780_Direct.h"

using namespace digitalcave;

#ifndef CHANNEL_COUNT
#define CHANNEL_COUNT					4
#endif

#define BUTTON_1						_BV(PORTD2)
#define BUTTON_2						_BV(PORTD3)

#define ENCODER1_CLOCKWISE				_BV(1)
#define ENCODER1_COUNTER_CLOCKWISE		_BV(2)
#define ENCODER2_CLOCKWISE				_BV(3)
#define ENCODER2_COUNTER_CLOCKWISE		_BV(4)

#define STATE_LOCKED					_BV(1)

static volatile uint8_t encoder_movement;

static Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, &PORTE, 6, &PORTE, 2, &PORTB, 7, &PORTD, 5, &PORTC, 6, &PORTC, 7);
static Display display(&hd44780, 4, 20);
static Buttons buttons(&PORTD, BUTTON_1 | BUTTON_2, 3, 8, 70, 8);
static uint8_t voltage_readings[CHANNEL_COUNT];
static uint8_t current_readings[CHANNEL_COUNT];

static uint8_t menu_state;

void read_values(){
	for(uint8_t i = 0; i < CHANNEL_COUNT; i++){
		voltage_readings[i] = analog_read_p(i * 2);
		current_readings[i] = analog_read_p((i * 2) + 1);
	}
}

void handle_interface(){
	_delay_ms(12);
	buttons.sample();
	
	uint8_t pressed = buttons.pressed();
	uint8_t held = buttons.held();

	if (pressed & BUTTON_1){
		menu_state ^= STATE_LOCKED;
	}
	
	if (encoder_movement){
		uint8_t encoder_temp = encoder_movement;
		encoder_movement = 0;
		
	}
}

void update_display(){
	display.write_text(1, 0, "                    ", 20);
	if (menu_state & STATE_LOCKED){
		display.write_text(0, 2, "Locked  ", 8);
	}
	else {
		display.write_text(0, 2, "Unlocked", 8);
	}
	
	display.refresh();
}

int main (void){
	//Enable pin change interrupts for encoders
	PCICR |= _BV(PCIE0);
	PCMSK0 |= 0x0F;	//Enable bits 0..3 for pin change interrupts
	PORTB |= _BV(PORTB0) | _BV(PORTB1);
	sei();
	
	//Start ADC
	uint8_t analog_pins[CHANNEL_COUNT * 2];
#if CHANNEL_COUNT > 0
	analog_pins[0] = 0;
	analog_pins[1] = 1;
#endif
#if CHANNEL_COUNT > 1
	analog_pins[2] = 4;
	analog_pins[3] = 5;
#endif
#if CHANNEL_COUNT > 2
	analog_pins[4] = 6;
	analog_pins[5] = 7;
#endif
#if CHANNEL_COUNT > 3
	analog_pins[6] = 8;
	analog_pins[7] = 9;
#endif
#if CHANNEL_COUNT > 4
	analog_pins[8] = 10;
	analog_pins[9] = 11;
#endif
#if CHANNEL_COUNT > 5
	analog_pins[10] = 12;
	analog_pins[11] = 13;
#endif
	//analog_init(analog_pins, CHANNEL_COUNT * 2, ANALOG_AVCC);

	//Debugging lights
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	DDRB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	
	_delay_ms(100);
	
	//Display init
	display.clear();
	display.write_text(1, 2, (char*) "Testing 1,2,3", 13);
	display.refresh();
	_delay_ms(1000);
	
	//Main program loop
	while (1){
		//TODO
		//if (usb_configured()){
		//	
		//}
		read_values();
		handle_interface();
		update_display();
	}
}

ISR(PCINT0_vect){
	static uint8_t encoder1 = 0x00;
	static uint8_t encoder2 = 0x00;
	
	encoder1 = ((encoder1 << 2) | (PINB & 0x03)) & 0x0F;
	encoder1 = ((encoder1 << 2) | ((PINB >> 2) & 0x03)) & 0x0F;
	
	switch(encoder1){
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			encoder_movement |= ENCODER1_CLOCKWISE;
			break;
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			encoder_movement |= ENCODER1_COUNTER_CLOCKWISE;
			break;
	}
	
	switch(encoder2){
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			encoder_movement |= ENCODER2_CLOCKWISE;
			break;
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			encoder_movement |= ENCODER2_COUNTER_CLOCKWISE;
			break;
	}
}