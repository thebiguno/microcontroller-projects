#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/analog/analog.h"
#include "lib/Button/Buttons.h"
#include "lib/Hd44780/Display.h"
#include "lib/Hd44780/Hd44780_Direct.h"

#include "Channel.h"

using namespace digitalcave;

#ifndef CHANNEL_COUNT
#define CHANNEL_COUNT					4
#endif

#if CHANNEL_COUNT > 6
#error The software does not currently support more than 4 channels.
#endif

#ifndef DISPLAY_ROWS
#define DISPLAY_ROWS					2
#endif

#ifndef DISPLAY_COLS
#define DISPLAY_COLS					16
#endif

#define MENU_COUNT						3

#define BUTTON_1						_BV(PORTD2)
#define BUTTON_2						_BV(PORTD3)

#define ENCODER1_CLOCKWISE				_BV(1)
#define ENCODER1_COUNTER_CLOCKWISE		_BV(2)
#define ENCODER2_CLOCKWISE				_BV(3)
#define ENCODER2_COUNTER_CLOCKWISE		_BV(4)

#define STATE_MENU_ITEM					_BV(5)
#define STATE_MENU						_BV(6)
#define STATE_LOCKED					_BV(7)


static volatile uint8_t encoder_movement;

static Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, &PORTE, 6, &PORTE, 2, &PORTB, 7, &PORTD, 5, &PORTC, 6, &PORTC, 7);
static Display display(&hd44780, DISPLAY_ROWS, DISPLAY_COLS);
static Buttons buttons(&PORTD, BUTTON_1 | BUTTON_2, 8, 8, 255, 0);

static Channel channels[CHANNEL_COUNT] = {
#if CHANNEL_COUNT > 0
	Channel(0, 15.0, 0.0, 0, 1.0, 0.0, 1),
#endif
#if CHANNEL_COUNT > 1
	Channel(1, 6.0, 2.0, 4, 0.5, 0.0, 5),
#endif
#if CHANNEL_COUNT > 2
	Channel(2, 12.0, 0.0, 6, 5.0, 0.0, 7),
#endif
#if CHANNEL_COUNT > 3
	Channel(3, 0.0, -12.5, 8, 1.0, 0.0, 9),
#endif
#if CHANNEL_COUNT > 4
	Channel(4, 0.0, -5.0, 10, 2.0, 0.0, 11),
#endif
#if CHANNEL_COUNT > 5
	Channel(5, 0.0, -3.5, 12, 0.0, -10.0, 13),
#endif
//You can add more channels if desired...
};

static uint8_t state;
static uint8_t state_selected_channel;	//Selects channel (bits 1..3) and voltage / current (bit 0)
static uint8_t state_menu_index;

void read_power_supply(){
	for(uint8_t i = 0; i < CHANNEL_COUNT; i++){
		channels[i].sample_actual();
	}
}

void read_user_input(){
//	_delay_ms(1);
	buttons.sample();
	
	uint8_t released = buttons.released();
	uint8_t held = buttons.held();
	
	//If we are not in menu mode, we can swap between locked and unlocked
	if (!(state & STATE_MENU) && held & BUTTON_1){
		state ^= STATE_LOCKED;
		if (state & STATE_LOCKED){
			hd44780.set_display(hd44780.DISPLAY_BLINK_OFF | hd44780.DISPLAY_CURSOR_OFF | hd44780.DISPLAY_ON);
		}
		else {
			hd44780.set_display(hd44780.DISPLAY_BLINK_ON | hd44780.DISPLAY_CURSOR_ON | hd44780.DISPLAY_ON);
			display.set_cursor_position(0, 0);
		}
	}
	//If we are not locked, we can swap between menu mode and normal mode
	else if ((state & STATE_LOCKED) && held & BUTTON_2){
		state ^= STATE_MENU;
		if (state & STATE_MENU){
			hd44780.set_display(hd44780.DISPLAY_BLINK_ON | hd44780.DISPLAY_CURSOR_ON | hd44780.DISPLAY_ON);
			display.set_cursor_position(0, 0);
		}
		else {
			hd44780.set_display(hd44780.DISPLAY_BLINK_OFF | hd44780.DISPLAY_CURSOR_OFF | hd44780.DISPLAY_ON);
		}
	}

	else if (released & BUTTON_1 || released & BUTTON_2){
		if ((state & STATE_LOCKED)){		//Locked mode
			
		}
		else {								//Unlocked mode
			if ((state & STATE_MENU)){		//Menu mode
				if (released & BUTTON_1){
					state_menu_index++;
					if (state_menu_index >= MENU_COUNT) state_menu_index = 0;
				}
				else if (released & BUTTON_2){
					state_menu_index--;
					if (state_menu_index >= MENU_COUNT) state_menu_index = MENU_COUNT - 1;
				}
			}
			else {							//Setpoint modification mode
				if (released & BUTTON_1){
					state_selected_channel++;
					if (state_selected_channel >= CHANNEL_COUNT * 2) state_selected_channel = 0;
				}
				else if (released & BUTTON_2){
					state_selected_channel--;
					if (state_selected_channel >= CHANNEL_COUNT * 2) state_selected_channel = (CHANNEL_COUNT * 2) - 1;
				}
			}
		}
		
	}
	
	else if (encoder_movement){
		uint8_t encoder_temp = encoder_movement;
		encoder_movement = 0;

		if ((state & STATE_LOCKED)){		//Locked mode
			if (encoder_temp & ENCODER1_CLOCKWISE){
				state_selected_channel += 2;
				if (state_selected_channel >= CHANNEL_COUNT * 2) state_selected_channel = CHANNEL_COUNT * 2 - 1;
			}
			else if (encoder_temp & ENCODER1_COUNTER_CLOCKWISE){
				state_selected_channel -= 2;
				if (state_selected_channel >= CHANNEL_COUNT * 2) state_selected_channel = 0;
			}
			
		}
		else {								//Unlocked mode
			if (state & STATE_MENU){		//Menu mode
				if (state & STATE_MENU_ITEM){
					
				}
				else {
					
				}
			}
			else {							//Setpoint modification mode
				Channel* channel = &channels[state_selected_channel >> 1];
				uint8_t selector = state_selected_channel & 0x01;
				//Modify the value
				if (encoder_temp & ENCODER1_CLOCKWISE) channel->adjust_setpoint(selector, 1);
				else if (encoder_temp & ENCODER1_COUNTER_CLOCKWISE) channel->adjust_setpoint(selector, -1);
				else if (encoder_temp & ENCODER2_CLOCKWISE) channel->adjust_setpoint(selector, 0.01);
				else if (encoder_temp & ENCODER2_COUNTER_CLOCKWISE) channel->adjust_setpoint(selector, -0.01);
			}
		}
	}
}

void update_display(){
	if (state & STATE_MENU){
		//TODO Show menu
	}
	else {		//Not menu mode
		char temp[20];
		uint8_t channel = state_selected_channel >> 1;
		if (channel > (CHANNEL_COUNT - DISPLAY_ROWS)){
			channel = (CHANNEL_COUNT - DISPLAY_ROWS);
		}

		for(uint8_t row = 0; row < DISPLAY_ROWS; row++){
			channels[channel].to_string((state & STATE_LOCKED), temp, DISPLAY_COLS + 1);
			display.write_text(row, 0, temp, DISPLAY_COLS);
			if ((channel == (state_selected_channel >> 1)) && !(state & STATE_LOCKED)){
				display.set_cursor_position(row, state_selected_channel & 0x01 ? 9 : 1);
			}
			channel++;
		}
	}
	
	display.refresh();
}

int main (void){
	//Enable pin change interrupts for encoders
	PCICR |= _BV(PCIE0);
	PCMSK0 |= 0x0F;	//Enable bits 0..3 for pin change interrupts
	PORTB |= _BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB3);
	sei();
	
// 	//Start ADC
// 	uint8_t analog_pins[CHANNEL_COUNT * 2];
// #if CHANNEL_COUNT > 0
// 	analog_pins[0] = 0;
// 	analog_pins[1] = 1;
// #endif
// #if CHANNEL_COUNT > 1
// 	analog_pins[2] = 4;
// 	analog_pins[3] = 5;
// #endif
// #if CHANNEL_COUNT > 2
// 	analog_pins[4] = 6;
// 	analog_pins[5] = 7;
// #endif
// #if CHANNEL_COUNT > 3
// 	analog_pins[6] = 8;
// 	analog_pins[7] = 9;
// #endif

	//analog_init(analog_pins, CHANNEL_COUNT * 2, ANALOG_AVCC);

	//Debugging lights
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	DDRB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	
	_delay_ms(100);
	
	//Display init
	display.clear();
	display.write_text(1, 2, "Testing 1,2,3", 13);
	display.refresh();
	state |= STATE_LOCKED;
	_delay_ms(1000);
	
	//Main program loop
	while (1){
		//TODO
		//if (usb_configured()){
		//	
		//}
		read_power_supply();
		read_user_input();
		update_display();
	}
}

ISR(PCINT0_vect){
	static uint8_t encoder1 = 0x00;
	static uint8_t encoder2 = 0x00;
	
	encoder1 = ((encoder1 << 2) | (PINB & 0x03)) & 0x0F;
	encoder2 = ((encoder2 << 2) | ((PINB >> 2) & 0x03)) & 0x0F;
	
	//The encoders I am using now output a full sequence for one step, so we only look for one
	// of the sub-sequences to avoid 4x faster movement than we want.  Change this as needed.
	switch(encoder1){
		case 0x01:
		//case 0x07:
		//case 0x08:
		//case 0x0E:
			encoder_movement |= ENCODER1_CLOCKWISE;
			break;
		case 0x02:
		//case 0x04:
		//case 0x0B:
		//case 0x0D:
			encoder_movement |= ENCODER1_COUNTER_CLOCKWISE;
			break;
	}
	
	switch(encoder2){
		case 0x01:
		//case 0x07:
		//case 0x08:
		//case 0x0E:
			encoder_movement |= ENCODER2_CLOCKWISE;
			break;
		case 0x02:
		//case 0x04:
		//case 0x0B:
		//case 0x0D:
			encoder_movement |= ENCODER2_COUNTER_CLOCKWISE;
			break;
	}
}