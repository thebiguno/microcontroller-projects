/*
 * Read the buttons on the PS2 Controller, and send them over the 
 * serial link.  We send all button events and changes to analog state.
 * We let the receiving end decide what to do with this information.
 * Initially designed for a hexapod controller, but it should be usable 
 * for most robotics / gaming interface tasks.
 *
 * There is an idle poll which re-sends button state on a regular basis
 * (default of about 500ms) even if there was no change of state.  This
 * can ensure that the receiving client does not miss an event.  The idle
 * polling does not re-send analog data.
 *
 * See lib/protocol/protocol.txt for protocol details and lib/universal_controller/protocol.txt 
 * for Universal Controller-specific commands.
 *
 * We announce the controller id ('U', for Universal Controller) every once in a while.
 *
 * By default, all 16 buttons and both joysticks are enabled.  To change this,
 * use the enable / disable button / joystick commands.
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <stdlib.h>

#include <Protocol.h>
#include <Serial_AVR.h>
#include <PSX_AVR.h>
#include <Hd44780_Direct.h>
#include <CharDisplay.h>

#include "lib/bootloader.h"

#include "Analog.h"

//Generic messages are in 0x0X space...
#define MESSAGE_ANNOUNCE_CONTROL_ID				0x00
#define MESSAGE_SEND_ACKNOWLEDGE				0x01
#define MESSAGE_SEND_COMPLETE					0x02
#define MESSAGE_REQUEST_ENABLE_DEBUG			0x03
#define MESSAGE_REQUEST_DISABLE_DEBUG			0x04
#define MESSAGE_SEND_DEBUG						0x05
#define MESSAGE_REQUEST_BATTERY					0x06
#define MESSAGE_SEND_BATTERY					0x07

//Universal Controller messages are in 0x1X space...
#define MESSAGE_UC_BUTTON_PUSH					0x10
#define MESSAGE_UC_BUTTON_RELEASE				0x11
#define MESSAGE_UC_BUTTON_NONE					0x12
#define MESSAGE_UC_JOYSTICK_MOVE				0x13
#define MESSAGE_UC_BUTTON_ENABLE				0x14
#define MESSAGE_UC_BUTTON_DISABLE				0x15
#define MESSAGE_UC_JOYSTICK_ENABLE				0x16
#define MESSAGE_UC_JOYSTICK_DISABLE				0x17
#define MESSAGE_UC_SET_POLL_FREQUENCY			0x18
#define MESSAGE_UC_SET_ANALOG_FREQUENCY			0x19


#define ADC_THROTTLE		13
#define ADC_BATTERY			12

#define THROTTLE_COUNT		2
#define POLL_COUNT			30
#define BATTERY_COUNT		100
#define BOOTLOADER_COUNT	125

using namespace digitalcave;

uint8_t last_LX = 0;
uint8_t last_LY = 0;
uint8_t last_RX = 0;
uint8_t last_RY = 0;

Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, 
	&PORTF, PORTF0,
	&PORTF, PORTF1,
	&PORTF, PORTF4,
	&PORTF, PORTF5,
	&PORTF, PORTF6,
	&PORTF, PORTF7);
CharDisplay display(&hd44780, 2, 16);

Protocol protocol;

PSX_AVR psx(&PORTD, PORTD4, //Data (Brown)
	&PORTD, PORTD5, //Command (Orange)
	&PORTD, PORTD6, //Clock (Blue)
	&PORTD, PORTD7); //Attention (Yellow)
	
Serial_AVR serial(38400, 8, 0, 1, 1, 32);

Analog analog;

int main (void){
	//Do setup here
	clock_prescale_set(clock_div_2);	//Run at 8MHz so that we can run on 3.3v
	
	uint8_t custom[64] = {
		0x0e,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,	// 0 (Battery Full)
		0x0e,0x11,0x11,0x11,0x1f,0x1f,0x1f,0x1f,	// 1 (Battery Half)
		0x0e,0x11,0x11,0x11,0x11,0x11,0x11,0x1f,	// 2 (Battery Empty)
		0x04,0x06,0x15,0x0e,0x0e,0x15,0x06,0x04,	// 3 (Bluetooth icon)
		0x19,0x19,0x1d,0x0c,0x06,0x17,0x13,0x13,	// 4 (XBee icon)
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 5 (Unused)
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 6 (Unused)
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 	// 7 (Unused)
	};
	display.set_custom_chars(custom);

	Message incoming(0);
	
	//Set up timer0 to output PWM for negative voltage
	DDRB |= _BV(PORTB7);
	TCCR0A = _BV(COM0A0) | _BV(WGM01);			//CTC Mode (mode 2), Toggle OC0A on compare match
	TCCR0B = _BV(CS02);							//Div 256 prescaler
	OCR0A = 1;									//Set compare value
	sei();
	
	uint8_t throttle_counter = 0;
	uint8_t battery_counter = 0;
	uint8_t bootloader_counter = 0;
	uint8_t poll_counter = 0;

	uint8_t throttle_position = 0;
	uint8_t battery_level = 0;
	
	char buf[17];	//String buffer, used for display formatting
	
	//Main program loop
	while (1){
		_delay_ms(10);

		poll_counter++;
		throttle_counter++;
		battery_counter++;
	
		if (throttle_counter > THROTTLE_COUNT){
			throttle_position = (analog.read(ADC_THROTTLE));
			throttle_counter = 0;
		}
		if (battery_counter > BATTERY_COUNT){
			battery_level = (analog.read(ADC_BATTERY));
			battery_counter = 0;
		}
		
		psx.poll();
		uint16_t buttons = psx.buttons();
		uint16_t changed = psx.changed();
		
		if (changed){
			for (uint8_t x = 0; x < 16; x++){
				//If this was triggered by a real button event, show the current state (newly pressed or newly released)
				if (changed & _BV(x)){
					if (buttons & _BV(x)){
						Message m(MESSAGE_UC_BUTTON_PUSH, &x, 1);
						protocol.write(&serial, &m);
					}
					else {
						Message m(MESSAGE_UC_BUTTON_RELEASE, &x, 1);
						protocol.write(&serial, &m);
					}
				}
			}
			
			poll_counter = 0;
		}
		
		if (poll_counter > POLL_COUNT){
			if (buttons){
				for (uint8_t x = 0; x < 16; x++){
					if (buttons & _BV(x)){
						Message m(MESSAGE_UC_BUTTON_PUSH, &x, 1);
						protocol.write(&serial, &m);
					}
				}
			}
			else {
				Message m(MESSAGE_UC_BUTTON_NONE, 0x00, 0);
				protocol.write(&serial, &m);
			}

			poll_counter = 0;
		}

		//Read any incoming bytes and handle completed messages if applicable
		if (protocol.read(&serial, &incoming)){
			//TODO
		}

		
		if (psx.button(PSB_SELECT)) display.write_text(0, 0, "Select          ", 16);
		else if (psx.button(PSB_L1)) display.write_text(0, 0, "Left 1          ", 16);
		else if (psx.button(PSB_L2)) display.write_text(0, 0, "Left 2          ", 16);
		else if (psx.button(PSB_L3)) display.write_text(0, 0, "Left 3          ", 16);
		else if (psx.button(PSB_R1)) display.write_text(0, 0, "Right 1         ", 16);
		else if (psx.button(PSB_R2)) display.write_text(0, 0, "Right 2         ", 16);
		else if (psx.button(PSB_R3)) display.write_text(0, 0, "Right 3         ", 16);
		else if (psx.button(PSB_START)) display.write_text(0, 0, "Start           ", 16);
		else if (psx.button(PSB_PAD_UP)) display.write_text(0, 0, "Pad Up          ", 16);
		else if (psx.button(PSB_PAD_LEFT)) display.write_text(0, 0, "Pad Left        ", 16);
		else if (psx.button(PSB_PAD_DOWN)) display.write_text(0, 0, "Pad Down        ", 16);
		else if (psx.button(PSB_PAD_RIGHT)) display.write_text(0, 0, "Pad Right       ", 16);
		else if (psx.button(PSB_TRIANGLE)) display.write_text(0, 0, "Triangle        ", 16);
		else if (psx.button(PSB_CIRCLE)) display.write_text(0, 0, "Circle          ", 16);
		else if (psx.button(PSB_CROSS)) display.write_text(0, 0, "Cross           ", 16);
		else if (psx.button(PSB_SQUARE)) display.write_text(0, 0, "Square          ", 16);
		else display.write_text(0, 0, "                ", 16);

		snprintf(buf, sizeof(buf), "%02X,%02X %02X,%02X %02X  ", psx.stick(PSS_LX), psx.stick(PSS_LY), psx.stick(PSS_RX), psx.stick(PSS_RY), throttle_position);
		display.write_text(1, 0, buf, 16);
		
		//Hold down circle + triangle and push left stick all the way up for a few seconds to enter bootloader mode
		if (psx.button(PSB_TRIANGLE) && psx.button(PSB_CIRCLE) && psx.stick(PSS_LY) == 0x00){
			if (bootloader_counter >= BOOTLOADER_COUNT){
				display.write_text(0, 0, "DFU Bootloader  ", 16);
				display.write_text(1, 0, "                ", 16);
				display.refresh();
				bootloader_jump();
			}
			bootloader_counter++;
		}
		else {
			bootloader_counter = 0;
		}
		
		display.refresh();
	}
}

ISR(USART1_RX_vect){
	serial.isr();
}