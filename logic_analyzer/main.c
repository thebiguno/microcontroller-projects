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

#include "lib/serial/serial.h"

//TODO This will be larger.
#define BUFFER_SIZE	256

uint8_t data[BUFFER_SIZE];

/*
 * Returns the size of free memory available for a buffer.
 */
uint16_t free_memory(){
	uint16_t temp = (uint16_t *) malloc(4);	//Allocate to get the heap pointer
	uint16_t hp = temp;
	free(temp);				//Release memory
	return SP - hp;			//Return the number of bytes free
}

void start_recording_data(){
	for (uint16_t i = 0; i < BUFFER_SIZE; i++){
		data[i] = PINA;
		
		//TODO delay depending on sample rate
	}
}

void dump_recorded_data(){
	for (uint16_t i = 0; i < BUFFER_SIZE; i++){
		serial_write_c(data[i]);
	}
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

	uint8_t command;
	uint8_t data[4];
	
	uint16_t free_memory = free_memory - 0x100;	//Keep 256 bytes free for stack

	while(1) {	
		//Read commands from client
		serial_read_c((char*) &command);

		switch(command){
			case 0x00:	//Reset
			case '0':
				//TODO Reset
				break;
				
			case 0x01:	//Arms the trigger
			case '1':
				start_recording_data();
				dump_recorded_data();
				break;
				
			case 0x02: //Ask for ID
			case '2':
				serial_write_c('1');
				serial_write_c('A');
				serial_write_c('L');
				serial_write_c('S');
				break;

			case 0x04:
			case '4': //Ask for ID
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
			case 0xC1:
			case 0xC2:
				//TODO Set trigger mask
				read_command_data(data);
				break;
				
//			case 0xC2:	//Set trigger config
			case 0xC6:
			case 0xCA:
			case 0xCE:
				//TODO Set trigger config
				read_command_data(data);
				break;
				
			case 0x80:	//Set divider
				read_command_data(data);
				divider = cmdBytes[2];
				divider = divider << 8;
				divider += cmdBytes[1];
				divider = divider << 8;
				divider += cmdBytes[0];
				
				if (divider >= 1500000) {
					useMicro = 0;
					delayTime = (divider + 1) / 100000;
				} 
				else {
					useMicro = 1;
					delayTime = (divider + 1) / 100;
				}
				break;
				
			case 0x81:	//Set read and delay count
				//TODO
				read_command_data(data);
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
