/*
 * Drum Master rev 2 - controller for up to 16 analog sensors.
 * Copyright 2014 - 2015 Wyatt Olson <wyatt.olson@gmail.com>
 * 
 * At a very high level, Drum Master will listen to a series of sensors and report the values back to the
 * Raspberry Pi via SPI.	Each signal is sent in a packet, using a binary protocol 
 * consisting of 2 bytes / packet:
 *
 *		|sscccckk|vvvvvvvv|
 *      <start:2><channel:4><checksum:2><velocity:8>
 *
 * Each portion of the packet is described below:
 *		<start> is a 2 bit flag 0x3 to signal the start of a new packet.
 *		<channel> is the 4 bit representation of a channel number between 0x00 and 0x0F (15).  
 *				Analog channels are 0..11, and digital channels are 12..15.
 *		<checksum> is the 2 bit checksum on the rest of the packet, calculated using the 
 *				2 bit parity word algorithm.  Each 2-bit chunk of the packet is XOR'd together. 
 *				The slave software will XOR all 8 2 bit words together; if the result is not 0x0
 *				then we know there was an error in transmission.
 *		<velocity> is the 8 bit representation of the actual analog value between 0x0 and 0xFF.
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


/*
 * Hardware mappings (breadboard version using Mega328):
 * -ADC Input is PC0
 * -MUX selector switches are PB0..PB3
 * -Drain enable switch is PB4
 */

/***** Serial Hardware *****/
void serial_init_b(uint32_t baud){
	//Set baud rate
	uint16_t calculated_baud = (F_CPU / 16 / baud) - 1;
	UBRR0H = calculated_baud >> 8;
	UBRR0L = calculated_baud & 0xFF;

	//Make sure 2x and multi processor comm modes are off
	UCSR0A &= ~(_BV(U2X0) | _BV(MPCM0));
	
	//8 Data bits, no parity, one stop bit
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	
	//Enable TX
	UCSR0B |= _BV(TXEN0);
}

void serial_write_c(char data){
	//Nop loop to wait until transmit buffer is ready to receive data
	while (!(UCSR0A & _BV(UDRE0)));

	UDR0 = data;

	//Wait for transmission to complete; this is required when the serial library is used
	// in conjunction with code that sleeps the CPU, such as synchronous ADC in sleep mode.
	while (!(UCSR0A & _BV(TXC0)));
	//Reset transmission complete flag
	UCSR0A &= _BV(TXC0);
}

/*
 * Sends data.  Channel is a 4 bit number, between 0 and 11 inclusive.  Velocity is a 8 bit number.
 */
void send_data(uint8_t channel, uint8_t velocity){
	if (channel > 12) return;
	uint8_t packet = 0xC0;	//Start bits
	uint8_t checksum = 0x3;	//0x03 is the checksum of the start bits
	
	//First packet consists of 6 channel bits, and (eventually) 2 checksum bits.
	packet |= ((channel << 0x02) & 0x3C);
	
	//Add channel to checksum
	checksum ^= (channel >> 0x02) & 0x03;
	checksum ^= channel & 0x3;
	
	//Add velocity to checksum
	checksum ^= (velocity >> 0x06) & 0x03;
	checksum ^= (velocity >> 0x04) & 0x03;
	checksum ^= (velocity >> 0x02) & 0x03;
	checksum ^= velocity & 0x03;
	
	packet |= checksum;
	serial_write_c(packet);

	//Second packet consists only of 8 bits data.
	serial_write_c(velocity);
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
	
	uint8_t velocity = ADCH;

	//Enable drain now that we have read the value
	PORTC |= _BV(PORTB4);
	
	if (velocity > 10){
		send_data(channel, velocity);
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
	serial_init_b(76800);
	
	//Init analog
	analog_init();
	
	while (1){
		
	}
}

