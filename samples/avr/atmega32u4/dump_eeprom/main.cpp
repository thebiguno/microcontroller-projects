#include <stdio.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include <SerialUSB.h>

using namespace digitalcave;

int main (){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	SerialUSB serial;

	while(!serial.isConnected());
	_delay_ms(2000);
	char temp[16];
	
	for (uint8_t i = 0; i < 64; i++){
		uint8_t size = snprintf(temp, sizeof(temp), "0x%02X:", i);
		serial.write((uint8_t*) temp, size);
		for (uint8_t j = 0; j < 16; j++){
			uint8_t b = eeprom_read_byte((uint8_t*) (uint16_t) (i * 16 + j));
			size = snprintf(temp, sizeof(temp), "  %02X", b);
			serial.write((uint8_t*) temp, size);
			eeprom_update_byte((uint8_t*) (uint16_t) (i * 16 + j), 0xFF);
		}
		serial.write('\n');
	}
	
	while (1){
	}
}
