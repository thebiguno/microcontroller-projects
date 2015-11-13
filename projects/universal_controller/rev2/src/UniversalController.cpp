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
 * We announce the controller id ('U', for Universal Controller) every once in a while.
 * 
 * The display reserves the two most right characters on both rows for status.
 * The farthest right char on the top row is the remote device's battery; the bottom row
 * is the Universal Controller's battery.  The second farthest right char on the top row
 * is the TX / RX status (flashes when TX / RX events happen); the bottom row is the
 * radio switch position (XBee or Bluetooth).
 * 
 * The rest of the display can be used for debug / status messages from the remote device.
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

#include <CharDisplay.h>
#include <Hd44780_Direct.h>
#include <FramedSerialProtocol.h>
#include <NullSerial.h>
#include <PSX_AVR.h>
#include <SerialAVR.h>
#include <SoftwareSerialAVR.h>

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


#define ADC_THROTTLE							13
#define ADC_BATTERY								12

#define BATTERY_FULL_THRESHOLD					170
#define BATTERY_EMPTY_THRESHOLD 				140

#define BATTERY_FULL_ICON						0
#define BATTERY_HALF_ICON						1
#define BATTERY_EMPTY_ICON						2

#define THROTTLE_COUNT							2
#define COMMUNICATION_COUNT						10
#define DIGITAL_POLL_COUNT						30
#define CONTRAST_COUNT							40
#define ANALOG_POLL_COUNT						50
#define BATTERY_COUNT							100
#define BOOTLOADER_COUNT						125

#define COMM_NONE								0x00
#define COMM_RX									0x01
#define COMM_TX									0x02
#define COMM_RX_TX								0x03


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

FramedSerialProtocol fsp(32);

PSX_AVR psx(&PORTD, PORTD4, //Data (Brown)
	&PORTD, PORTD5, //Command (Orange)
	&PORTD, PORTD6, //Clock (Blue)
	&PORTD, PORTD7); //Attention (Yellow)
	
SerialAVR serialAvr(38400, 8, 0, 1, 1);
SoftwareSerialAVR softwareSerialAvr(&PORTE, PORTE6, 9600);
NullSerial nullSerial;

Analog analog;

uint8_t contrast_counter = 0xFF;
uint8_t throttle_counter = 0;
uint8_t battery_counter = 0;
uint8_t bootloader_counter = 0;
uint8_t analog_poll_counter = 0;
uint8_t digital_poll_counter = 0;
uint8_t communication_counter = 0;

uint8_t communication = COMM_NONE;
uint8_t throttle_position = 0;
uint8_t battery_level = 0;

char buf[15];	//String buffer, used for display formatting

char text_line[14] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};	

Serial* serial;		//Pointer to which serial port (tx) we are currently using

void sendMessage(FramedSerialMessage* m){
	fsp.write(serial, m);
	communication |= COMM_TX;
	communication_counter = 0;
}

int main (void){
	//Do setup here
	clock_prescale_set(clock_div_2);	//Run at 8MHz so that we can run on 3.3v
	
	uint8_t custom[64] = {
		0x0e,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,	// 0 (Battery Full)
		0x0e,0x11,0x11,0x11,0x1f,0x1f,0x1f,0x1f,	// 1 (Battery Half)
		0x0e,0x11,0x11,0x11,0x11,0x11,0x11,0x1f,	// 2 (Battery Empty)
		0x04,0x06,0x15,0x0e,0x0e,0x15,0x06,0x04,	// 3 (Bluetooth icon)
		0x19,0x19,0x1d,0x0c,0x06,0x17,0x13,0x13,	// 4 (XBee icon)
		0x00,0x00,0x00,0x00,0x06,0x05,0x06,0x05,	// 5 (Rx)
		0x1c,0x08,0x08,0x08,0x00,0x00,0x00,0x00,	// 6 (Tx)
		0x1c,0x08,0x08,0x08,0x06,0x05,0x06,0x05 	// 7 (Rx + Tx)
	};
	display.set_custom_chars(custom);

	FramedSerialMessage incoming(0, 32);
	
	//Set up timer0 to output PWM for negative voltage generation
	DDRB |= _BV(PORTB7);
	TCCR0A = _BV(COM0A0) | _BV(WGM01);			//CTC Mode (mode 2), Toggle OC0A on compare match
	TCCR0B = _BV(CS01) | _BV(CS00);				//Div 64 prescaler
	OCR0A = 5;									//Set compare value
	sei();
	
	//Turn the switch sensors' pullups on
	PORTC |= _BV(PORTC6) | _BV(PORTC7);
	
	//Main program loop
	while (1){
		_delay_ms(10);
		

		analog_poll_counter++;
		digital_poll_counter++;
		throttle_counter++;
		battery_counter++;
		communication_counter++;
	
		if (throttle_counter > THROTTLE_COUNT){
			throttle_position = (analog.read(ADC_THROTTLE));
			throttle_counter = 0;
		}
		if (battery_counter > BATTERY_COUNT){
			uint8_t reading = (analog.read(ADC_BATTERY));
			if (battery_level == 0 && reading < (BATTERY_FULL_THRESHOLD - 5)) battery_level = BATTERY_HALF_ICON;			//Move from full to med
			else if (battery_level == 1 && reading >= (BATTERY_FULL_THRESHOLD + 5)) battery_level = BATTERY_FULL_ICON;		//Move from med to full
			else if (battery_level == 1 && reading <= (BATTERY_EMPTY_THRESHOLD - 5)) battery_level = BATTERY_EMPTY_ICON;	//Move from med to empty
			else if (battery_level == 2 && reading >= (BATTERY_EMPTY_THRESHOLD + 5)) battery_level = BATTERY_HALF_ICON;		//Move from empty to med
			battery_counter = 0;
			
			//Request battery from remote device
			FramedSerialMessage m(MESSAGE_REQUEST_BATTERY, 0x00, 0);
			sendMessage(&m);

		}
		if (communication_counter > COMMUNICATION_COUNT){
			communication = COMM_NONE;
			communication_counter = 0;
		}
		
		//Read the switch state
		if ((PINC & _BV(PORTC6)) == 0){
			serial = &softwareSerialAvr;
		}
		else if ((PINC & _BV(PORTC7)) == 0){
			serial = &serialAvr;
		}
		else {
			serial = &nullSerial;
		}

		
		psx.poll();
		
		uint16_t buttons = psx.buttons();
		uint16_t changed = psx.changed();
		
		//Digital transmission section
		if (changed){
			for (uint8_t x = 0; x < 16; x++){
				//If this was triggered by a real button event, show the current state (newly pressed or newly released)
				if (changed & _BV(x)){
					if (buttons & _BV(x)){
						FramedSerialMessage m(MESSAGE_UC_BUTTON_PUSH, &x, 1);
						sendMessage(&m);
					}
					else {
						FramedSerialMessage m(MESSAGE_UC_BUTTON_RELEASE, &x, 1);
						sendMessage(&m);
					}
				}
			}
			
			digital_poll_counter = 0;
		}
		
		if (digital_poll_counter > DIGITAL_POLL_COUNT){
			if (buttons){
				for (uint8_t x = 0; x < 16; x++){
					if (buttons & _BV(x)){
						FramedSerialMessage m(MESSAGE_UC_BUTTON_PUSH, &x, 1);
						sendMessage(&m);
					}
				}
			}
			else {
				FramedSerialMessage m(MESSAGE_UC_BUTTON_NONE, 0x00, 0);
				sendMessage(&m);
			}

			digital_poll_counter = 0;
		}
		
		//Analog transmission section
		if (changed){
			//TODO write analog values
			analog_poll_counter = 0;
		}
		if (analog_poll_counter > ANALOG_POLL_COUNT){
			//TODO write analog values
		}

//		if (psx.button(PSB_SELECT)) display.write_text(0, 0, "Select        ", 14);
//		else if (psx.button(PSB_L1)) display.write_text(0, 0, "Left 1        ", 14);
//		else if (psx.button(PSB_L2)) display.write_text(0, 0, "Left 2        ", 14);
//		else if (psx.button(PSB_L3)) display.write_text(0, 0, "Left 3        ", 14);
//		else if (psx.button(PSB_R1)) display.write_text(0, 0, "Right 1       ", 14);
//		else if (psx.button(PSB_R2)) display.write_text(0, 0, "Right 2       ", 14);
//		else if (psx.button(PSB_R3)) display.write_text(0, 0, "Right 3       ", 14);
//		else if (psx.button(PSB_START)) display.write_text(0, 0, "Start         ", 14);
//		else if (psx.button(PSB_PAD_UP)) display.write_text(0, 0, "Pad Up        ", 14);
//		else if (psx.button(PSB_PAD_LEFT)) display.write_text(0, 0, "Pad Left      ", 14);
//		else if (psx.button(PSB_PAD_DOWN)) display.write_text(0, 0, "Pad Down      ", 14);
//		else if (psx.button(PSB_PAD_RIGHT)) display.write_text(0, 0, "Pad Right     ", 14);
//		else if (psx.button(PSB_TRIANGLE)) display.write_text(0, 0, "Triangle      ", 14);
//		else if (psx.button(PSB_CIRCLE)) display.write_text(0, 0, "Circle        ", 14);
//		else if (psx.button(PSB_CROSS)) display.write_text(0, 0, "Cross         ", 14);
//		else if (psx.button(PSB_SQUARE)) display.write_text(0, 0, "Square        ", 14);
//		else display.write_text(0, 0, "                ", 16);
//
//		snprintf(buf, sizeof(buf), "%02X,%02X %02X,%02X %02X  ", psx.stick(PSS_LX), psx.stick(PSS_LY), psx.stick(PSS_RX), psx.stick(PSS_RY), throttle_position);
//		display.write_text(1, 0, buf, 16);
		
		//Show local battery level
		display.write_text(1, 15, battery_level);
		
		//Read any incoming bytes and handle completed messages if applicable
		if (fsp.read(&serialAvr, &incoming)){
			communication |= COMM_RX;
			communication_counter = 0;
			//TODO
			
			switch(incoming.getCommand()){
				case MESSAGE_SEND_BATTERY:
					if (incoming.getData()[0] >= 170) display.write_text(0, 15, BATTERY_FULL_ICON);
					else if (incoming.getData()[0] >= 85) display.write_text(0, 15, BATTERY_HALF_ICON);
					else display.write_text(0, 15, BATTERY_EMPTY_ICON);
					break;
				case MESSAGE_SEND_DEBUG:
					//Copy the last received line to the display's second line
					display.write_text(1, 0, text_line, 14);
					//Copy the newly received message into the text buffer
					for (uint8_t i = 0; i < incoming.getLength() && i < 14; i++){
						text_line[i] = incoming.getData()[i];
					}
					//Show the newly received line on the display's first line
					display.write_text(0, 0, text_line, 14);
					break;
			}
		}
		
		//Show radio icons according to serial device / switch state
		if (serial == &softwareSerialAvr){
			display.write_text(1, 14, (char) 0x04);	//XBee
		}
		else if (serial == &serialAvr){
			display.write_text(1, 14, (char) 0x03);	//Bluetooth
		}
		else {
			display.write_text(1, 14, (char) 0xFE);	//Nothing
		}
		
		//Show Rx / Tx Status
		if (communication == COMM_RX_TX){
			display.write_text(0, 14, (char) 0x07);	//Rx + Tx
		}
		else if (communication == COMM_TX){
			display.write_text(0, 14, (char) 0x06);	//Tx
		}
		else if (communication == COMM_RX){
			display.write_text(0, 14, (char) 0x05);	//Rx
		}
		else {
			display.write_text(0, 14, (char) 0xFE);	//Nothing
		}
		
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
		
		if (psx.button(PSB_TRIANGLE) && psx.button(PSB_CIRCLE) && psx.button(PSB_PAD_UP)){
			if (contrast_counter > CONTRAST_COUNT){
				OCR0A--;
				contrast_counter = 0;
			}
			contrast_counter++;
		}
		else if (psx.button(PSB_TRIANGLE) && psx.button(PSB_CIRCLE) && psx.button(PSB_PAD_DOWN)){
			if (contrast_counter > CONTRAST_COUNT){
				OCR0A++;
				contrast_counter = 0;
			}
			contrast_counter++;
		}
		else {
			contrast_counter = 0xFF;
		}
		
		display.refresh();
	}
}

ISR(USART1_RX_vect){
	serialAvr.isr();
}