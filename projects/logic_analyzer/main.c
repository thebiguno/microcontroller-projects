/*
 * Implementation of SUMP logic analyser.  See protocol details 
 * at http://www.sump.org/projects/analyzer/protocol/ .
 *
 * Hardware:
 * This runs on a ATMega1284 chip (or compatible).  The 1284 is preferable
 * due to its large SRAM (16k, vs the 644's 4k); the 1284 (or higher) is
 * recommended.
 * 
 * This implementation uses PORTA as the sampler, so thus supports up to 8 channels.
 * You should run on a 20MHz clock, and connect a FTDI cable to the serial port.
 * 
 * TODO: 
 * - Input protection on inputs (zener diodes, 1k resistors (?), etc)
 * - Pulldown resistors (10k?) on inputs to avoid floating
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

/*
 * Returns the size of free memory available for a buffer.
 */
uint16_t free_memory(){
	uint16_t *temp = (uint16_t *) malloc(4);	//Allocate to get the heap pointer
	uint16_t *hp = temp;
	free(temp);				//Release memory
	return SP - *hp;			//Return the number of bytes free
}

void read_command_data(uint8_t* data){
	serial_read_c((char*) data);
	serial_read_c((char*) data + 1);
	serial_read_c((char*) data + 2);
	serial_read_c((char*) data + 3);
}


int main() {
	serial_init_b(57600);
	
	DDRA = 0x00;	//All PORTA is input
	DDRB = 0xFF;	//PORTB is output

	uint8_t command;
	uint8_t trigger_mask;
	uint8_t trigger_values;
	uint8_t trigger_config;
	uint8_t data[4];
	uint16_t sample_count = 512;	//Requested sample count.  If this is higher than the buffer size, we will return blank values once buffer runs out.
	uint16_t sample_count_after_trigger = 512;		//Requested sample count after trigger.
	uint16_t buffer_size = 0;
	uint32_t divider = 0;
	uint8_t use_micro = 0;
	uint16_t delay_time = 0;
	
	while(1) {
		PORTB = 0x01;
	
		//Read commands from client
		serial_read_c((char*) &command);

		PORTB = 0x00;

		switch(command){
			case 0x00:	//Reset
				break;
				
			case '1':
			case 0x01:	//Arms the trigger / start recording
			case 0x03:	//Test mode: return sawtooth wave
				PORTB = 0x2;
				buffer_size = free_memory() - 0x400;	//Keep 1k bytes free for more stack allocations.  We can adjust this with experimentation.
				if (buffer_size > sample_count) buffer_size = sample_count;
			
				//Allocate buffer
				uint8_t *buffer = (uint8_t *) malloc(buffer_size);
			
				PORTB = 0x4;
			
				if (command == 0x01){
					for (uint16_t i = 0; i < buffer_size; i++){
						buffer[i] = PINA;
						
						//TODO delay depending on sample rate
					}
				}
				else if (command == 0x03){
					//For self test we write a sawtooth waveform
					uint8_t value = 0x00;
					for (uint16_t i = 0; i < buffer_size; i++){
						buffer[i] = value++;
					}				
				}
				
				PORTB = 0x8;

				//Write the buffer back in reverse...
				for (uint16_t i = sample_count - 1; i >= 0; i--){
					if (i < buffer_size) serial_write_c(buffer[i]);
					else serial_write_c(0x00);
				}
				
				PORTB = 0x10;
				
				//Release buffer
				free(buffer);

				break;
				
			case 0x02: //Ask for ID
				serial_write_c('1');
				serial_write_c('A');
				serial_write_c('L');
				serial_write_c('S');
				break;

			case 0x04:	//Metadata
				//Device name
				serial_write_c(0x01);
				serial_write_c('A');
				serial_write_c('V');
				serial_write_c('R');
				serial_write_c('L');
				serial_write_c('A');
				serial_write_c('0');
				serial_write_c(0x00);
				
				/* sample memory (64) */
				serial_write_c(0x21);
				serial_write_c(0x00);
				serial_write_c(0x04);
				serial_write_c(0x00);
				serial_write_c(0x00);
				
				/* sample rate (1MHz) */
				serial_write_c(0x23);
				serial_write_c(0x00);
				serial_write_c(0x0F);
				serial_write_c(0x42);
				serial_write_c(0x40);
				
				/* number of probes = 8 */
				serial_write_c(0x40);
				serial_write_c(0x08);
				
				/* protocol version (2) */
				serial_write_c(0x41);
				serial_write_c(0x02);
				
				/* end of data */
				serial_write_c(0x00); 
				break;
				
			case 0x11:	//XON
				//Not implemented
				break;
			
			case 0x13: //XOFF
				//Not implemented
				break;
				
			case 0xC0:	//Set trigger mask
				read_command_data(data);
				trigger_mask = data[0];
				break;
				
			case 0xC1:	//Set trigger values
				read_command_data(data);
				trigger_values = data[0];
				break;

			case 0xC2:	//Set trigger config
				read_command_data(data);
				trigger_config = data[0];
				break;
								
			case 0x80:	//Set divider
				read_command_data(data);

				divider = data[2];
				divider = divider << 8;
				divider += data[1];
				divider = divider << 8;
				divider += data[0];
				
				if (divider >= 1500000) {
					use_micro = 0;
					delay_time = (divider + 1) / 100000;
				} 
				else {
					use_micro = 1;
					delay_time = (divider + 1) / 100;
				}
				break;
				
			case 0x81:	//Set read and delay count
				read_command_data(data);
				
				//Figure out how many samples there should be before and after the trigger fires.
				sample_count = 4 * ((data[1] << 8) | data[0]) + 1;
				sample_count_after_trigger = 4 * ((data[3] << 8) | data[2]) + 1;
								
				break;
				
			case 0x82:	//Set flags
				//TODO
				read_command_data(data);
				break;
				
			default:
				//Do nothing; optionally consume 4 data bytes if it is an extended command
				if (command >= 0x80) read_command_data(data);
				break;
		}
	}
}
