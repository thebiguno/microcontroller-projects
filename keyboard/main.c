/*
 * Keyboard driver.  Can drive a 8x16 keyboard matrix, plus has ICSP and USB HID output.
 * Uses an ATMega124 (32 I/O) running at 20MHz (requires a crystal).
 * 
 * Pinouts: 
 *		PORTA: 8x Row Drivers
 *		PORTB: 8x Column Readers + ICSP breakout
 *		PORTC: 8x Column Readers
 *		PORTD: Misc I/O
 *			D0, D1:	Serial Breakout (Rx Tx)
 *			D2, D3:	USB Data D+ / D- (via level converters)
 *			D4..D6:	Shift Register Outputs for LEDs
 *			D7: Unused?
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

/*
 * Sets the row (0..7)
 */
void set_row(uint8_t row){
	PORTA = ~(_BV(row));
}

/*
 * Reads and debounces the column.  High bits indicate a key press.
 */
uint16_t read_columns(){
	uint16_t result = PINC;
	result = result << 8;
	result += PINB;
	result = ~result;
	return result;
}

int main (void){
	serial_init_b(9600);
	
	//Set rows into low impedence (write) mode
	DDRA = 0xFF;
	
	//Ensure columns are high impedence (read) with pullups
	DDRB = 0x00;
	DDRC = 0x00;
	PORTB = 0xFF;
	PORTC = 0xFF;

	//Used as itoa destination
	char temp[32];

	uint8_t row = 0x00;
	uint16_t columns = 0x00;
	
	//Main program loop
	while (1){
		set_row(row);
		_delay_ms(1);
		columns = read_columns();
	
		if (columns != 0x00){
			for (uint8_t i = 0; i < 16; i++){
				if (_BV(i) & columns){
					serial_write_s("Scan Code:\t");
					serial_write_s(itoa(row, temp, 16));
					serial_write_s("\t");
					serial_write_s(itoa(i, temp, 16));
//					serial_write_s(itoa(columns, temp, 16));
					serial_write_s("\n\r");
					_delay_ms(100);
				}
			}
		}
	
		row++;
		if (row > 7) row = 0;
	}
}
