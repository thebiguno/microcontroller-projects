#include <util/delay.h>

#include <Serial_AVR.h>

using namespace digitalcave;

int main (){
	Serial_AVR serial(38400, 8, 0, 1, 1);
	
	while (1){
		serial.write('F');
		serial.write('o');
		serial.write('o');
		serial.write('\n');
		
		_delay_ms(1000);
	}   
}
