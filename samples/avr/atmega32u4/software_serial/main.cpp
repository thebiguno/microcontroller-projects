#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <SoftwareSerialAVR.h>

using namespace digitalcave;

int main (){
	clock_prescale_set(clock_div_2);
	
	SoftwareSerialAVR serial(&PORTE, PORTE6, 9600);
	
	while (1){
		serial.write('F');
		serial.write('o');
		serial.write('o');
		serial.write('\n');
		
		_delay_ms(1000);
	}   
}
