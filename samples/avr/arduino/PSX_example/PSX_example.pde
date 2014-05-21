#include <util/delay.h>

#define CTRL_CLK        20
#define CTRL_BYTE_DELAY 20

//These are our button constants
#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_GREEN       0x1000
#define PSB_RED         0x2000
#define PSB_BLUE        0x4000
#define PSB_PINK        0x8000

//These are stick values
#define PSS_RX 5
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8


void setup(){
  Serial.begin(115200);
  
  psx_init(&PORTD, &PIND, &DDRD, PORTD6, //Data (Brown)
    &PORTD, &DDRD, PORTD5, //Clock (Blue)
    &PORTD, &DDRD, PORTD7, //Command (Orange)
    &PORTB, &DDRB, PORTB0); //Attention (Yellow)
}

void loop(){
    psx_read_gamepad();
  
    if(psx_button(PSB_SELECT))
     Serial.print("Select\n"); 
     
    if (psx_stick(PSS_LY) != 0xFF)
    Serial.print(psx_stick(PSS_LY), HEX); //Left stick, Y axis. Other options: LX, RY, RX

    delay(100);     
}
















//Define ports used
volatile uint8_t *_psx_data_port = 0;
volatile uint8_t *_psx_clock_port = 0;
volatile uint8_t *_psx_command_port = 0;
volatile uint8_t *_psx_attention_port = 0;

//Define the input
volatile uint8_t *_psx_data_in = 0;

//Define ddr used
volatile uint8_t *_psx_data_ddr = 0;
volatile uint8_t *_psx_clock_ddr = 0;
volatile uint8_t *_psx_command_ddr = 0;
volatile uint8_t *_psx_attention_ddr = 0;

//Define pins to use
uint8_t _psx_clock_pin = 0;
uint8_t _psx_command_pin = 0;
uint8_t _psx_attention_pin = 0;
uint8_t _psx_data_pin = 0;

//Common structure for last read data
uint8_t _psx_data[9];

void psx_init(volatile uint8_t *data_port, volatile uint8_t *data_in, volatile uint8_t *data_ddr, uint8_t data_pin,
				volatile uint8_t *clock_port, volatile uint8_t *clock_ddr, uint8_t clock_pin,
				volatile uint8_t *command_port, volatile uint8_t *command_ddr, uint8_t command_pin,
				volatile uint8_t *attention_port, volatile uint8_t *attention_ddr, uint8_t attention_pin){
	//Store the ports...
	_psx_data_port = data_port;
	_psx_clock_port = clock_port;
	_psx_command_port = command_port;
	_psx_attention_port = attention_port;
	
	//... and data in....
	_psx_data_in = data_in;

	//... and output registers...
	_psx_data_ddr = data_ddr;
	_psx_clock_ddr = clock_ddr;
	_psx_command_ddr = command_ddr;
	_psx_attention_ddr = attention_ddr;

	//... and pin numbers.
	_psx_clock_pin = clock_pin;
	_psx_command_pin = command_pin;
	_psx_attention_pin = attention_pin;
	_psx_data_pin = data_pin;
	
        //Init by polling once
        psx_read_gamepad();
//        psx_read_gamepad();
//        psx_read_gamepad();
	
	//Set clock, attention, and command pins to output mode
	*_psx_clock_ddr |= _BV(_psx_clock_pin);
	*_psx_attention_ddr |= _BV(_psx_attention_pin);
	*_psx_command_ddr |= _BV(_psx_command_pin);

	//Set data pin to input mode, and set pullup resistor
	*_psx_data_ddr &= ~(_BV(_psx_data_pin));
	*_psx_data_port |= _BV(_psx_data_pin);

	//Initialize game pad
	*_psx_clock_port |= _BV(_psx_clock_pin);
	*_psx_command_port |= _BV(_psx_command_pin);

	//Enable joystick
	//Note: before you submit each command packet, you must set attention low; once
	// you are done each packet, return it high.  You have to toggle the line before
	// you submit another command.
	*_psx_attention_port &= ~_BV(_psx_attention_pin);
	_psx_gamepad_shift(0x01);
	_psx_gamepad_shift(0x43);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x01);
	_psx_gamepad_shift(0x00);
    *_psx_attention_port |= _BV(_psx_attention_pin);



	// Lock to Analog Mode on Stick
	*_psx_attention_port &= ~(_BV(_psx_attention_pin));	
	_psx_gamepad_shift(0x01);
	_psx_gamepad_shift(0x44);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x01);
	_psx_gamepad_shift(0x03);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x00);
	*_psx_attention_port |= _BV(_psx_attention_pin);
	
	// Vibration (disabled; need to define vibration pins if you want to enable it)
	/*
	*_psx_attention_port &= ~(_BV(_psx_attention_pin));	
	_gamepad_shiftout(0x01);
	_gamepad_shiftout(0x4D);
	_gamepad_shiftout(0x00);
	_gamepad_shiftout(0x00);
	_gamepad_shiftout(0x01);
	*_psx_attention_port |= _BV(_psx_attention_pin);
	*/
    
	//Config controller to return all pressure values
	/*	
	*_psx_attention_port &= ~(_BV(_psx_attention_pin));	
	_psx_gamepad_shift(0x01);
	_psx_gamepad_shift(0x4F);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0xFF);
	_psx_gamepad_shift(0xFF);
	_psx_gamepad_shift(0x03);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x00);
	*_psx_attention_port |= _BV(_psx_attention_pin);
	*/

	//Exit config mode
	*_psx_attention_port &= ~(_BV(_psx_attention_pin));	
	_psx_gamepad_shift(0x01);
	_psx_gamepad_shift(0x43);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x00);
	_psx_gamepad_shift(0x5A);
	_psx_gamepad_shift(0x5A);
	_psx_gamepad_shift(0x5A);
	_psx_gamepad_shift(0x5A);
	_psx_gamepad_shift(0x5A);
	*_psx_attention_port |= _BV(_psx_attention_pin);
}


uint8_t psx_button(uint16_t button) {
	uint16_t buttons = *(uint16_t*)(_psx_data + 3); //Get 2 bytes, comprising data positions 3 and 4.
	return ((~buttons & button) > 0);
}

uint8_t psx_stick(unsigned int stick) {
	return _psx_data[stick];
}

/* The actual serial transfer.  Handles clock.  The PSX controller is full duplex,
 * so this will send a byte as well as receive one.
 */
uint8_t _psx_gamepad_shift(uint8_t byte) {
	uint8_t tmp = 0;
	for(uint8_t i = 0; i < 8; i++) {
		if (byte & (_BV(i))) {
			*_psx_command_port |= _BV(_psx_command_pin);
		}
		else {
			*_psx_command_port &= ~_BV(_psx_command_pin);
		}
		*_psx_clock_port &= ~_BV(_psx_clock_pin);
		_delay_us(CTRL_CLK);
		if(*_psx_data_in & _BV(_psx_data_pin)) {
			tmp |= _BV(i);
		}
		*_psx_clock_port |= _BV(_psx_clock_pin);
	}

	*_psx_command_port |= _BV(_psx_command_pin);
	_delay_us(CTRL_BYTE_DELAY);
/*
Serial.print(byte, HEX);
Serial.print(" ");
Serial.println(tmp, HEX);
*/
	return tmp;
}

void psx_read_gamepad() {
	*_psx_command_port |= _BV(_psx_command_pin);
	*_psx_clock_port |= _BV(_psx_clock_pin);
	*_psx_attention_port &= ~_BV(_psx_attention_pin); // Get controller attention
	_delay_us(CTRL_BYTE_DELAY);
	
	//Send the command to send button and joystick data;
	char dword[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	for (uint8_t i = 0; i < 9; i++) {
		_psx_data[i] = _psx_gamepad_shift(dword[i]);
	}
	*_psx_attention_port |= _BV(_psx_attention_pin); // Done communication
}

