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

#include <Serial_AVR.h>
#include <PSX_AVR.h>
#include <Hd44780_Direct.h>
#include <CharDisplay.h>

#include "lib/analog.h"
#include "lib/bootloader.h"

#define ADC_THROTTLE		0
#define ADC_BATTERY			1

#define THROTTLE_COUNT		10
#define BATTERY_COUNT		100
#define BOOTLOADER_COUNT	125

using namespace digitalcave;

uint8_t last_LX = 0;
uint8_t last_LY = 0;
uint8_t last_RX = 0;
uint8_t last_RY = 0;

int main (void){
	//Do setup here
	clock_prescale_set(clock_div_2);	//Run at 8MHz so that we can run on 3.3v
	
	Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, 
		&PORTF, PORTF0,
		&PORTF, PORTF1,
		&PORTF, PORTF4,
		&PORTF, PORTF5,
		&PORTF, PORTF6,
		&PORTF, PORTF7);
	CharDisplay display(&hd44780, 2, 16);
	uint8_t custom[64] = {
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,			// 0 (Use 0xFE)
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1f,			// 1
		0x0,0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,			// 2
		0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,			// 3
		0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,		// 4
		0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,		// 5
		0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,		// 6
		0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f		// 7
													// 8 (Use 0xFF)
	};
	display.set_custom_chars(custom);

	PSX_AVR psx(&PORTD, PORTD4, //Data (Brown)
		&PORTD, PORTD5, //Command (Orange)
		&PORTD, PORTD6, //Clock (Blue)
		&PORTD, PORTD7); //Attention (Yellow)
		
	Serial_AVR serial(9600, 8, 0, 1, 1);
	
	uint8_t analog_pins[2];
	analog_pins[ADC_THROTTLE] = 0x25;	//ADC13
	analog_pins[ADC_BATTERY] = 0x24;	//ADC12
	analog_init(analog_pins, 2, ANALOG_AVCC);
	ADMUX |= _BV(ADLAR);
	
	//Set up timer0 to output PWM for negative voltage
	DDRB |= _BV(PORTB7);
	TCCR0A = _BV(COM0A0) | _BV(WGM01);			//CTC Mode (mode 2), Toggle OC0A on compare match
	TCCR0B = _BV(CS02);							//Div 256 prescaler
	OCR0A = 1;									//Set compare value
	sei();
	
	uint8_t throttle_counter = 0;
	uint8_t battery_counter = 0;
	uint8_t bootloader_counter = 0;

	uint8_t throttle_position = 0;
	uint8_t battery_level = 0;
	
 	char buf[17];
	
	//Main program loop
	while (1){
		_delay_ms(10);
		
		if (throttle_counter > THROTTLE_COUNT){
			throttle_position = (analog_read_p(ADC_THROTTLE) >> 8);
			throttle_counter = 0;
		}
		if (battery_counter > BATTERY_COUNT){
			battery_level = (analog_read_p(ADC_BATTERY) >> 8);
			battery_counter = 0;
		}
		throttle_counter++;
		battery_level++;
		
		psx.poll();
		
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
		
		//Hold down circle + triangle and push left stick all the way up for more than 2.5s to enter bootloader mode
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
