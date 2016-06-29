#include <dcutil/delay.h>
#include <Arduino.h>

void delay_ms(uint32_t d){
	delay(d);
}

void delay_us(uint32_t d){
	delayMicroseconds(d);
}
