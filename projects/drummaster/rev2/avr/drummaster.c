/*
 * Drum Master rev 2 - controller for up to 16 analog sensors (12 are peak detecting circuits, 4 are simple ADC readings).
 * Copyright 2014 - 2015 Wyatt Olson <wyatt.olson@gmail.com>
 * 
 * At a very high level, Drum Master will listen to a series of sensors and report the values back to the
 * Raspberry Pi via serial connection.
 *
 * Slave software, running on the computer, must take these data packets and map them to digital
 * audio samples, based on the channel, velocity, and current state of the program.
 *
 * For more information, please visit http://drummaster.digitalcave.ca.
 * 
 * Changelog:
 * 2.0 - December 2014 - ?:   -Started work on complete rewrite for Rev 2.0.  Includes new hardware, new software, new architecture.  More to follow.
 */

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "lib/protocol/protocol.h"
#include "lib/serial/serial.h"

/*
 * Hardware mappings (breadboard version using Mega328):
 * -ADC Input is PC0
 * -MUX selector switches are PB0..PB3
 * -Drain enable switch is PB4
 */

void protocol_dispatch_message(uint8_t cmd, uint8_t *bytes, uint8_t length){
	if (cmd == 0x32){
		uint8_t kit_id[1] = {0x00};	//TODO Read last used kit ID from EEPROM
		protocol_send_message(0x33, kit_id, 1);
	}
}

/***** ADC Hardware *****/
void analog_init(){
	//Disable digital inputs from ADC channel 0.
	DIDR0 = _BV(ADC0D);

	//Reference of AVCC with external capacitor, Left Adjust Result, MUX channel = 0
	ADMUX = _BV(REFS0) | _BV(ADLAR);
	
	//ADC in Free Running mode
	ADCSRB = 0x00;
	//ADC Enable, Enable ADC Interrupts, /64 Prescaler -- Start first conversion
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADSC);
}

/*
 * The main work is done in the ADC ISR.  Here we read the last result, send it to 
 * the computer, and start a new conversion.
 */
ISR(ADC_vect){
	static uint8_t channel = 0;
	static uint8_t velocity[1];
	
	velocity[0] = ADCH;

	//Enable drain now that we have read the value
	PORTC |= _BV(PORTB4);
	
	if (velocity[0] > 10){
		protocol_send_message(channel, velocity, 1);
	}
	else {
		_delay_us(200);
	}
	
	//Disable the drain
	PORTC &= ~_BV(PORTB4);
	
	channel++;
	if (channel >= 12) channel = 0;
	
	//Set the new channel
	PORTC |= channel;
	
	//Start a conversion
	ADCSRA |= _BV(ADSC);
}

int main (void){
	//Init serial
	serial_init_b(38400);
	
	//Init analog
	//analog_init();
	
	while (1){
		_delay_ms(1000);
		uint8_t velocity[1] = {0xC9};
		protocol_send_message(0, velocity, 1);
	}
}

