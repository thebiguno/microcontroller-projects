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
#include "timer.h"

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
#define MESSAGE_UC_THROTTLE_MOVE				0x1A


#define ADC_THROTTLE							13
#define ADC_BATTERY								12

#define THROTTLE_THRESHOLD						4
#define STICK_THRESHOLD							4

#define BATTERY_FULL_THRESHOLD					160
#define BATTERY_EMPTY_THRESHOLD 				100

#define BATTERY_FULL_ICON						0
#define BATTERY_HALF_ICON						1
#define BATTERY_EMPTY_ICON						2

//Times (in milliseconds) before various events happen
#define THROTTLE_TIME							1000
#define COMMUNICATION_TIME						100
#define ANNOUNCE_TIME							2000
#define DIGITAL_POLL_TIME						1000
#define CONTRAST_TIME							500
#define ANALOG_POLL_TIME						1000
#define BATTERY_TIME							2000
#define BOOTLOADER_TIME							1000

#define COMM_NONE								0x00
#define COMM_RX									0x01
#define COMM_TX									0x02
#define COMM_RX_TX								0x03

#define AVERAGE_DIVISOR_EXPONENT				3


using namespace digitalcave;

//Running average of analog values; divide by 64 (2^AVERAGE_DIVISOR_EXPONENT) for actual value
uint16_t average_stick[4] = {0, 0, 0, 0};
uint16_t average_throttle = 0;
//Last sent values
uint8_t last_stick[4] = {0, 0, 0, 0};
uint8_t last_throttle = 0;

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

uint32_t contrast_timer = 0;
uint32_t announce_timer = 0;
uint32_t throttle_timer = 0;
uint32_t battery_timer = 0;
uint32_t bootloader_timer = 0;
uint32_t analog_poll_timer = 0;
uint32_t digital_poll_timer = 0;
uint32_t communication_timer = 0;

uint8_t communication = COMM_NONE;
uint8_t battery_level = 0;

char buf[15];	//String buffer, used for display formatting

char text_line[14] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};	

Serial* serial;		//Pointer to which serial port (tx) we are currently using

void sendMessage(FramedSerialMessage* m){
	fsp.write(serial, m);
	communication |= COMM_TX;
	communication_timer = timer_millis();
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
	
	//Timer 1 is the millis counter
	timer_init();
	
	//Turn the switch sensors' pullups on
	PORTC |= _BV(PORTC6) | _BV(PORTC7);
	
	//Main program loop
	while (1){
		uint32_t time = timer_millis();
	
		if ((time - battery_timer) > BATTERY_TIME){
			uint8_t reading = (analog.read(ADC_BATTERY));
			if (battery_level == 0 && reading < (BATTERY_FULL_THRESHOLD - 5)) battery_level = BATTERY_HALF_ICON;			//Move from full to med
			else if (battery_level == 1 && reading >= (BATTERY_FULL_THRESHOLD + 5)) battery_level = BATTERY_FULL_ICON;		//Move from med to full
			else if (battery_level == 1 && reading <= (BATTERY_EMPTY_THRESHOLD - 5)) battery_level = BATTERY_EMPTY_ICON;	//Move from med to empty
			else if (battery_level == 2 && reading >= (BATTERY_EMPTY_THRESHOLD + 5)) battery_level = BATTERY_HALF_ICON;		//Move from empty to med
			battery_timer = time;
			
			//Request battery from remote device
			FramedSerialMessage m(MESSAGE_REQUEST_BATTERY, 0x00, 0);
			sendMessage(&m);

		}
		if ((time - communication_timer) > COMMUNICATION_TIME){
			communication = COMM_NONE;
			communication_timer = time;
		}
		if ((time - announce_timer) > ANNOUNCE_TIME){
			//Send device ID
			uint8_t id = 'U';
			FramedSerialMessage m(MESSAGE_ANNOUNCE_CONTROL_ID, &id, 1);
			sendMessage(&m);

			announce_timer = time;
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
		
		//Hold down square + cross and push left stick all the way down for a second or so to send the 'start' button press
// 		if (psx.button(PSB_CROSS) && psx.button(PSB_SQUARE) && (average_stick[1] >> AVERAGE_DIVISOR_EXPONENT) == 0xFF){
// 			if ((time - start_timer) >= START_TIME){
// 				buttons |= PSB_START;
// 				changed |= PSB_START;
// 				start_timer = time;
// 			}
// 			start_counter++;
// 		}
// 		else {
// 			start_counter = 0;
// 		}
		
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
			
			digital_poll_timer = time;
		}
		
		//Button repeat
		if ((time - digital_poll_timer) > DIGITAL_POLL_TIME){
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

			digital_poll_timer = time;
		}
		
		//Analog transmission section
		
		//Add in the current analog values to the running average
		average_stick[0] = average_stick[0] + psx.stick(PSS_LX) - (average_stick[0] >> AVERAGE_DIVISOR_EXPONENT);
		average_stick[1] = average_stick[1] + psx.stick(PSS_LY) - (average_stick[1] >> AVERAGE_DIVISOR_EXPONENT);
		average_stick[2] = average_stick[2] + psx.stick(PSS_RX) - (average_stick[2] >> AVERAGE_DIVISOR_EXPONENT);
		average_stick[3] = average_stick[3] + psx.stick(PSS_RY) - (average_stick[3] >> AVERAGE_DIVISOR_EXPONENT);
		
		average_throttle = average_throttle + analog.read(ADC_THROTTLE) - (average_throttle >> AVERAGE_DIVISOR_EXPONENT);
		
		if ((time - analog_poll_timer) > ANALOG_POLL_TIME
				|| abs((average_stick[0] >> AVERAGE_DIVISOR_EXPONENT) - last_stick[0]) > STICK_THRESHOLD
				|| abs((average_stick[1] >> AVERAGE_DIVISOR_EXPONENT) - last_stick[1]) > STICK_THRESHOLD
				|| abs((average_stick[2] >> AVERAGE_DIVISOR_EXPONENT) - last_stick[2]) > STICK_THRESHOLD
				|| abs((average_stick[3] >> AVERAGE_DIVISOR_EXPONENT) - last_stick[3]) > STICK_THRESHOLD
				|| abs((average_throttle >> AVERAGE_DIVISOR_EXPONENT) - last_throttle) > THROTTLE_THRESHOLD){

			last_stick[0] = (average_stick[0] >> AVERAGE_DIVISOR_EXPONENT);
			last_stick[1] = (average_stick[1] >> AVERAGE_DIVISOR_EXPONENT);
			last_stick[2] = (average_stick[2] >> AVERAGE_DIVISOR_EXPONENT);
			last_stick[3] = (average_stick[3] >> AVERAGE_DIVISOR_EXPONENT);
			last_throttle = average_throttle >> AVERAGE_DIVISOR_EXPONENT;

			FramedSerialMessage sticks(MESSAGE_UC_JOYSTICK_MOVE, last_stick, 4);
			sendMessage(&sticks);
			
			FramedSerialMessage throttle(MESSAGE_UC_THROTTLE_MOVE, &last_throttle, 1);
			sendMessage(&throttle);
			
			analog_poll_timer = time;
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
			communication_timer = time;
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
		if (psx.button(PSB_TRIANGLE) && psx.button(PSB_CIRCLE) && (average_stick[1] >> AVERAGE_DIVISOR_EXPONENT) == 0x00){
			if ((time - bootloader_timer) >= BOOTLOADER_TIME){
				display.write_text(0, 0, "DFU Bootloader  ", 16);
				display.write_text(1, 0, "                ", 16);
				display.refresh();
				bootloader_jump();
			}
		}
		else {
			bootloader_timer = time;
		}
		
		if (psx.button(PSB_TRIANGLE) && psx.button(PSB_CIRCLE) && psx.button(PSB_PAD_UP)){
			if ((time - contrast_timer) > CONTRAST_TIME){
				OCR0A--;
				contrast_timer = time;
			}
		}
		else if (psx.button(PSB_TRIANGLE) && psx.button(PSB_CIRCLE) && psx.button(PSB_PAD_DOWN)){
			if ((time - contrast_timer) > CONTRAST_TIME){
				OCR0A++;
				contrast_timer = time;
			}
		}
		else {
			contrast_timer = time;
		}
		
		display.refresh();
	}
}

ISR(USART1_RX_vect){
	serialAvr.isr();
}