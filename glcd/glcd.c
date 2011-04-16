#include "glcd.h"

static volatile uint8_t *_data_port = 0;
static volatile uint8_t *_clock_port = 0;
static volatile uint8_t *_a0_port = 0;
static volatile uint8_t *_cs1b_port = 0;
static volatile uint8_t *_reset_port = 0;

static uint8_t _data_pin = 0;
static uint8_t _clock_pin = 0;
static uint8_t _a0_pin = 0;
static uint8_t _cs1b_pin = 0;
static uint8_t _reset_pin = 0;

/*
 * Send a software SPI command.  See page 24 of driver datasheet
 */
void _glcd_do(uint8_t a0, uint8_t data){
	//Bring CS1B low (just to be sure...)
	*_cs1b_port &= ~(_BV(_cs1b_pin));
	
	//If a0 = 1, then we are outputting data; a0 low is a command.
	if (a0){
		*_a0_port |= _BV(_a0_pin);
	}
	else {
		*_a0_port &= ~(_BV(_a0_pin));
	}
	
	//Send the data
	for(int8_t i = 7; i >= 0; i--) {
		//Drop the clock...
		*_clock_port &= ~_BV(_clock_pin);
	
		//...set the command (outgoing) pin...
		if (data & (_BV(i))) {
			*_data_port |= _BV(_data_pin);
		}
		else {
			*_data_port &= ~_BV(_data_pin);
		}
		
		//...wait half the clock cycle...
		_delay_us(CTRL_CLK);
		
		//...raise the clock to HIGH...
		*_clock_port |= _BV(_clock_pin);
		
		//...and wait the other half of the clock cycle
		_delay_us(CTRL_CLK);
	}	
	
	//Bring CS1B high
//	*_cs1b_port |= _BV(_cs1b_pin);
}

void _glcd_command(uint8_t data){
	_glcd_do(0, data);
}

void _glcd_data(uint8_t data){
	_glcd_do(1, data);
}

/*
 * Hardware restart.  Pulls the reset pin low, waits, then brings it high again.
 * See ST7565R Driver datasheet page 40 for information.
 */
void glcd_hardware_reset(){
	//Bring reset low (should be low already, unless init has been called again)
	*_reset_port &= ~(_BV(_reset_pin));
	
	//Wait 100ms for power to stabilize, then bring the reset pin high (/reset)
	_delay_ms(100);
	*_reset_port |= _BV(_reset_pin);
	
	//See ST7565R Driver datasheet page 51
	_delay_ms(1);
}


void glcd_init(volatile uint8_t *data_port, uint8_t data_pin, 
		volatile uint8_t *clock_port, uint8_t clock_pin, 
		volatile uint8_t *a0_port, uint8_t a0_pin,
		volatile uint8_t *cs1b_port, uint8_t cs1b_pin,
		volatile uint8_t *reset_port, uint8_t reset_pin){

	_data_port = data_port;
	_clock_port = clock_port;
	_a0_port = a0_port;
	_cs1b_port = cs1b_port;
	_reset_port = reset_port;

	//Set up all pins as output; PORTX - 0x1 is DDRX
	*(_data_port - 0x1) |= _BV(data_pin);
	*(_clock_port - 0x1) |= _BV(clock_pin);
	*(_a0_port - 0x1) |= _BV(a0_pin);
	*(_cs1b_port - 0x1) |= _BV(cs1b_pin);
	*(_reset_port - 0x1) |= _BV(reset_pin);
	
	_data_pin = data_pin;
	_clock_pin = clock_pin;
	_a0_pin = a0_pin;
	_cs1b_pin = cs1b_pin;
	_reset_pin = reset_pin;
	
	//ST7565R Driver datasheet says that "When the power is turned on, the IC 
	// internal state becomes unstable, and it is necessary to initialize it 
	// using the /RES terminal" (page 40).
	glcd_hardware_reset();
	
	//See driver datasheet page 51 for required init sequence
	_glcd_command(0xA2);	//Set LCD bias to 1/9 (page 43)
	_glcd_command(0xA0);	//ADC Select (page 43)
	_glcd_command(0xC0);	//Common output mode select (page 45)
	_glcd_command(0x24);	//V0 voltage regulator internal resistor ratio set to 5.0 (page 45, 34)
	//TODO Electronic volume control?
	//TODO Power controller set?
	
	_glcd_command(0xAF);	//LCD On
}



void do_something(){
/*
	write_command(0xA0);	//ADC Select (Normal)
	write_command(0xAE);	//LCD Off
	write_command(0xC0);	//Common Output mode selection (Normal)
	write_command(0xA2);	//LCD Bias Set (1/9 bias)
	write_command(0x2F);	//Power control operating mode 7
	write_command(0x26);	//Resistor ratio 6
	write_command(0x81);	//Display start line to 0x1
	write_command(0x2F);	//Power control operating mode 7

	write_command(0xA5);	//All points on
	
	_delay_ms(500);
	
	write_command(0xA4);	//All points off (normal)
	*/

	
	
	_glcd_data(0xAA);		//Write 0b01010101
	_glcd_data(0xAA);		//Write 0b01010101
	_glcd_data(0xAA);		//Write 0b01010101
	_glcd_data(0xAA);		//Write 0b01010101
	_glcd_data(0xAA);		//Write 0b01010101
	_glcd_data(0xAA);		//Write 0b01010101

}