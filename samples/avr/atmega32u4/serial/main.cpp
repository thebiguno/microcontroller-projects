#include <util/delay.h>

#include <SerialAVR.h>

using namespace digitalcave;

SerialAVR serial(38400, 8, 0, 1, 1);
int main (){
	DDRB |= _BV(PORTB4);
	PORTB |= _BV(PORTB4);
	
	sei();
	uint8_t b;
	
	while (1){
		serial.write('F');
		serial.write('o');
		serial.write('o');
		serial.write('\n');
		
		if (serial.read(&b)) PORTB ^= _BV(PORTB4);
		
		_delay_ms(1000);
	}   
}

ISR(USART1_RX_vect){
	serial.isr();
}