/******************************************************************
*  PS2 controller library for AVR
*
*    Original code by Shutter on Arduino Forums
*
*    Revamped, added analog stick support and made into Arduino library by:
*              Bill Porter
*              http://www.billporter.info/?p=219
*
*    Converted to generic AVR library without C++ or Arduino references, and 
*              the ability to use any pins for input / output by:
*              Wyatt Olson
*              http://digitalcave.ca
*
*  Lib version history
*    ????		0.1		Bill Porter		-Made into library, added analog stick support
*    2010-07	0.2		Wyatt Olson		-Converted to generic AVR-GCC library
*                       				-Added port / pin initializers (psx_init())
*										-Changed to use uintx_t data types instead of char
*
*
* Usage information:
*  Connect PSX controller wires to AVR:
*		Yellow: Attention
*		Orange: Command
*		Brown:  Data
*		Blue:	Clock
*  Call psx_init() function with the port / pins which you are using
*  Call psx_button() function with PSB_X argument to read the value of the given button (see
*              psx.h for possible buttons, e.g. PSB_SELECT, PSB_L3, PSB_PAD_UP, etc)
*  Call psx_stick() function with PSS_X argument to read the analog stick value (possible
*              values are PSS_RX, PSS_RY, PSS_LX, PSS_LY for right / left and X / Y)
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
	//Store the port and output registers for future reference.
	_psx_port = psx_port;
	_psx_ddr = psx_ddr;

	//Store the pin numbers.
	_psx_clk_pin = clk_pin;
	_psx_cmd_pin = cmd_pin;
	_psx_att_pin = att_pin;
	_psx_dat_pin = dat_pin;
	
	
	//Set clock, attention, and command pins to output mode
	*_psx_ddr |= _BV(_psx_clk_pin) | _BV(_psx_att_pin) | _BV(_psx_cmd_pin);
    //Set data pin to input mode, and set pullup resistor
    *_psx_ddr &= ~(_BV(_psx_dat_pin));
    *_psx_port |= _BV(_psx_dat_pin);

  	//Initialize game pad
	*_psx_port |= _BV(_psx_cmd_pin);
	*_psx_port |= _BV(_psx_clk_pin);
	*_psx_port &= ~_BV(_psx_att_pin); // low enable joystick
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
	*_psx_port |= _BV(_psx_att_pin);
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
		if (byte & (_BV(i))) {
			*_psx_port |= _BV(_psx_cmd_pin);
		}
		else {
			*_psx_port &= ~_BV(_psx_cmd_pin);
		}
		*_psx_port &= ~_BV(_psx_clk_pin);
		_delay_us(CTRL_CLK);
		if(*_psx_ddr & _BV(_psx_dat_pin)) {
			tmp |= _BV(i);
		}
		*_psx_port |= _BV(_psx_clk_pin);
	}
	*_psx_port |= _BV(_psx_cmd_pin);
	_delay_us(CTRL_BYTE_DELAY);
	return tmp;
}
void _psx_gamepad_shiftout (uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		if(byte & _BV(i)) {
			*_psx_port |= _BV(_psx_cmd_pin);
		}
		else {
			*_psx_port &= ~_BV(_psx_cmd_pin);
		}
		*_psx_port &= ~_BV(_psx_clk_pin);
		_delay_us(CTRL_CLK);
		*_psx_port |= _BV(_psx_clk_pin);
	}
	*_psx_port |= _BV(_psx_cmd_pin);
	_delay_us(CTRL_BYTE_DELAY);
}
uint8_t _psx_gamepad_shiftin() {
	unsigned char tmp = 0;
	for (uint8_t i = 0; i < 8; i++) {
		*_psx_port &= ~_BV(_psx_cmd_pin);
		*_psx_port &= ~_BV(_psx_clk_pin);
		_delay_us(CTRL_CLK);
		if(*_psx_ddr & _BV(_psx_dat_pin)) {
			tmp |= _BV(i);
		}
		*_psx_port |= _BV(_psx_clk_pin);
		_delay_us(CTRL_CLK);
	}
	*_psx_port |= _BV(_psx_cmd_pin);
	_delay_us(CTRL_BYTE_DELAY);
	return tmp;
}
void psx_read_gamepad() {
    *_psx_port |= _BV(_psx_cmd_pin);
    *_psx_port |= _BV(_psx_clk_pin);
    *_psx_port &= ~_BV(_psx_att_pin); // low enable joystick
    _delay_us(CTRL_BYTE_DELAY);
    
    //Send the command to send button and joystick data;
    char dword[9] = {0x01,0x42,0,0,0,0,0,0,0};
	for (uint8_t i = 0; i<9; i++) {
		_psx_data[i] = _psx_gamepad_shiftinout(dword[i]);
    }
    *_psx_port |= _BV(_psx_att_pin); // HI disable joystick
}
uint8_t _psx_get_gamepad_mode() {
    *_psx_port |= _BV(_psx_cmd_pin);
    *_psx_port |= _BV(_psx_clk_pin);
    *_psx_port &= ~_BV(_psx_att_pin); // low enable joystick
    _psx_gamepad_shiftout(0x01);
    unsigned char x = _psx_gamepad_shiftin();
    *_psx_port |= _BV(_psx_att_pin); // HI disable joystick
    return x;
}
