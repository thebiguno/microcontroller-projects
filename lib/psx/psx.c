/******************************************************************
*  Super amazing PS2 controller Arduino Library v0.1
*		details and example sketch: 
*			http://www.billporter.info/?p=219
*
*    Original code by Shutter on Arduino Forums
*
*    Revamped, added analog stick support and made into lib by:
*              Bill Porter
*              www.billporter.info
*    Further changed by Wyatt Olson to include ability to define pins, and
*              to remove Arduino-dependent code and convert to standard C
*              instead of C++
*              Wyatt Olson
*              wyatt@digitalcave.ca
*
*  Lib version history
*    0.1 made into library, added analog stick support. 
*    0.2 Conversions by Wyatt, adding pin / port initializers (2010-07)
*
******************************************************************/


#include "psx.h"

/* Define the ports, outputs, and pins used */
volatile uint8_t *_psx_port = 0;
volatile uint8_t *_psx_ddr = 0;

uint8_t _psx_clk_pin = 0;
uint8_t _psx_cmd_pin = 0;
uint8_t _psx_att_pin = 0;
uint8_t _psx_dat_pin = 0;

uint8_t _psx_data[9];

void psx_init(volatile uint8_t *psx_port, volatile uint8_t *psx_ddr, 
				uint8_t clk_pin, 	
				uint8_t cmd_pin, 
				uint8_t att_pin, 
				uint8_t dat_pin){
	_psx_port = psx_port;
	_psx_ddr = psx_ddr;

	_psx_clk_pin = clk_pin;
	_psx_cmd_pin = cmd_pin;
	_psx_att_pin = att_pin;
	_psx_dat_pin = dat_pin;
	
	*_psx_ddr |= 1 << _psx_clk_pin;
	*_psx_ddr |= 1 << _psx_att_pin;
	*_psx_ddr |= 1 << _psx_cmd_pin;
    //Set data pin to input mode
    *_psx_ddr &= ~(1 << _psx_dat_pin);
    *_psx_port |= 1 << _psx_dat_pin;

  
  
	*_psx_port |= 1 << _psx_cmd_pin;
	*_psx_port |= 1 << _psx_clk_pin;
	*_psx_port &= ~(1 << _psx_att_pin); // low enable joystick
    _psx_gamepad_shiftout(0x01);
    _psx_gamepad_shiftout(0x43);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x01);
    _psx_gamepad_shiftout(0x00);
    // Lock to Analog Mode on Stick
    _psx_gamepad_shiftout(0x01);
    _psx_gamepad_shiftout(0x44);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x01);
    _psx_gamepad_shiftout(0x03);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x00);
    // Vibration
    /*
    _gamepad_shiftout(0x01);
    _gamepad_shiftout(0x4D);
    _gamepad_shiftout(0x00);
    _gamepad_shiftout(0x00);
    _gamepad_shiftout(0x01);
    */
    _psx_gamepad_shiftout(0x01);
    _psx_gamepad_shiftout(0x4F);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0xFF);
    _psx_gamepad_shiftout(0xFF);
    _psx_gamepad_shiftout(0x03);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x01);
    _psx_gamepad_shiftout(0x43);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x00);
    _psx_gamepad_shiftout(0x5A);
    _psx_gamepad_shiftout(0x5A);
    _psx_gamepad_shiftout(0x5A);
    _psx_gamepad_shiftout(0x5A);
    _psx_gamepad_shiftout(0x5A);
	*_psx_port |= 1 << _psx_att_pin;
}


uint8_t psx_button(uint16_t button) {
    uint16_t buttons = *(uint16_t*)(_psx_dat_pin+3);
    return ((~buttons & button) > 0);
}

uint8_t psx_stick(unsigned int stick) {
  return _psx_data[stick];
}

uint8_t _psx_gamepad_shiftinout (uint8_t byte) {
	uint8_t tmp = 0;
	for(uint8_t i = 0; i < 8; i++) {
		if (byte & (1 << i)) {
			*_psx_port |= 1 << _psx_cmd_pin;
		}
		else {
			*_psx_port &= ~(1 << _psx_cmd_pin);
		}
		*_psx_port &= ~(1 << _psx_clk_pin);
		_delay_us(CTRL_CLK);
		if(*_psx_ddr & (1 << _psx_dat_pin)) {
			tmp |= 1 << i;
		}
		*_psx_port |= 1 << _psx_clk_pin;
	}
	*_psx_port |= 1 << _psx_cmd_pin;
	_delay_us(CTRL_BYTE_DELAY);
	return tmp;
}
void _psx_gamepad_shiftout (uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		if(byte & (1 << i)) {
			*_psx_port |= 1 << _psx_cmd_pin;
		}
		else {
			*_psx_port &= ~(1 << _psx_cmd_pin);
		}
		*_psx_port &= ~(1 << _psx_clk_pin);
		_delay_us(CTRL_CLK);
		*_psx_port |= 1 << _psx_clk_pin;
	}
	*_psx_port |= 1 << _psx_cmd_pin;
	_delay_us(CTRL_BYTE_DELAY);
}
uint8_t _psx_gamepad_shiftin() {
	unsigned char tmp = 0;
	for (uint8_t i = 0; i < 8; i++) {
		*_psx_port &= ~(1 << _psx_cmd_pin);
		*_psx_port &= ~(1 << _psx_clk_pin);
		_delay_us(CTRL_CLK);
		if(*_psx_ddr & (1 << _psx_dat_pin)) {
			tmp |= 1 << i;
		}
		*_psx_port |= 1 << _psx_clk_pin;
		_delay_us(CTRL_CLK);
	}
	*_psx_port |= 1 << _psx_cmd_pin;
	_delay_us(CTRL_BYTE_DELAY);
	return tmp;
}
void psx_read_gamepad() {
    *_psx_port |= 1 << _psx_cmd_pin;
    *_psx_port |= 1 << _psx_clk_pin;
    *_psx_port &= ~(1 << _psx_att_pin); // low enable joystick
    _delay_us(CTRL_BYTE_DELAY);
    //Send the command to send button and joystick data;
    char dword[9] = {0x01,0x42,0,0,0,0,0,0,0};
	for (uint8_t i = 0; i<9; i++) {
		_psx_data[i] = _psx_gamepad_shiftinout(dword[i]);
    }
    *_psx_port |= 1 << _psx_att_pin; // HI disable joystick
}
uint8_t _psx_get_gamepad_mode() {
    *_psx_port |= 1 << _psx_cmd_pin;
    *_psx_port |= 1 << _psx_clk_pin;
    *_psx_port &= ~(1 << _psx_att_pin); // low enable joystick
    _psx_gamepad_shiftout(0x01);
    unsigned char x = _psx_gamepad_shiftin();
    *_psx_port |= 1 << _psx_att_pin; // HI disable joystick
    return x;
}
