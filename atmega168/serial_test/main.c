#include <util/delay.h>
#include "../../lib/serial/serial.h"

int main (){
	serial_init_b(57600);
		
	char s;
	
	while (1){
		while (serial_read_c(&s)){
			serial_write_c(s);
		}
		
		serial_write_s("Foo Bar!\n");
		
		//_delay_ms(1000);
	}   
}
