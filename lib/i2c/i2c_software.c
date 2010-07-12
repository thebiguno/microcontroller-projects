/* 
 * Software bit-banging for a simplified subset of i2c.
 * See http://codinglab.blogspot.com/2008/10/i2c-on-avr-using-bit-banging.html 
 * for original code.
 * 
 * Modified by Wyatt Olson to allow specification of port / pins at init time, 
 * along with code cleanup.
 *
 */



volatile uint8_t *_i2c_ddr = 0;
volatile uint8_t *_i2c_pin = 0;
volatile uint8_t *_i2c_port = 0;

uint8_t i2c_clock = 0;
uint8_t i2c_data = 0;

/*
 * Functions to set data pin / clock HIGH or LOW (depending on mode).
 * We (presumably) use DDR rather than PORT because if we switch 
 */
void _i2c_data(uint8_t mode){
	if (mode) *_i2c_ddr &= ~_BV(i2c_data);
	else *_i2c_ddr |= _BV(i2c_data);
}

void _i2c_clock(uint8_t mode){
	if (mode) *_i2c_ddr &= ~_BV(i2c_clock);
	else *_i2c_ddr |= _BV(i2c_clock);
}


void _i2c_write_bit(uint8_t c){
    if ( c > 0 ) {
        _i2c_data(1);
    }
    else {
        _i2c_data(0);
    }

    _i2c_clock(1);
    _delay_ms(1);

    _i2c_clock(0);
    _delay_ms(1);

    if ( c > 0 ) {
        _i2c_data(0);
    }

    _delay_ms(1);
   
}

uint8_t _i2c_read_bit(){
    _i2c_data(1);

    _i2c_clock(1);
    _delay_ms(1);

    unsigned char c = _i2c_pin;

    _i2c_clock(0);
    _delay_ms(1);

    return ( c >> I2C_DAT ) & 1;
}

/*
 * Specify which port / pins you want to use for i2c bit banging, and initializes the
 * hardware.  Must be called before the other functions.
 */
void i2c_init(volatile uint8_t *i2c_ddr, volatile uint8_t *i2c_pin, volatile uint8_t *i2c_port, 
		uint8_t i2c_clock, uint8_t i2c_data){
		
	_i2c_ddr = i2c_ddr;
	_i2c_pin = i2c_pin;
	_i2c_port = i2c_port;
	_i2c_data = i2c_data;
	_i2c_clock = i2c_clock;
		
    _i2c_port &= ~(_BV(_i2c_data) | _BV(_i2c_clock));

    _i2c_clock(1);
    _i2c_data(1);

    _delay_ms(1);
}

/* Send a START Condition */
void i2c_start(){
    // set both to high at the same time
    _i2c_ddr &= ~(_BV(_i2c_data) | _BV(_i2c_clock));
    _delay_ms(1);
   
    _i2c_data(0);
    _delay_ms(1);

    _i2c_clock(0);
    _delay_ms(1);
}

/* Send a STOP Condition */
void i2c_stop(){
    _i2c_clock(1);
    _delay_ms(1);

    _i2c_data(1);
    _delay_ms(1);
}

/*
 * Writes a byte to a device.  Must be called after i2c_start.
 */
uint8_t i2c_write(uint8_t c){
    for (uint8_t i = 0; i < 8; i++) {
        _i2c_write_bit(c & 128);
		c <<= 1;
    }

    return _i2c_read_bit();
}


/* 
 * Reads a byte from the I2C slave device, must be called after i2c_start.
 */
uint8_t i2c_read(uint8_t ack){
    unsigned char res = 0;

    for (uint8_t i = 0; i < 8; i++) {
        res <<= 1;
        res |= _i2c_read_bit();  
    }

    if (ack > 0) _i2c_write_bit(0);
    else _i2c_write_bit(1);

    _delay_ms(1);

    return res;
}
