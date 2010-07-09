//See http://codinglab.blogspot.com/2008/10/i2c-on-avr-using-bit-banging.html for sample code

// Port for the I2C
#define I2C_DDR DDRD
#define I2C_PIN PIND
#define I2C_PORT PORTD

// Pins to be used in the bit banging
#define I2C_CLK 0    
#define I2C_DAT 1

#define I2C_DATA_HI() I2C_DDR &= ~( 1 << I2C_DAT );
#define I2C_DATA_LO() I2C_DDR |=  ( 1 << I2C_DAT );

#define I2C_CLOCK_HI() I2C_DDR &= ~( 1 << I2C_CLK );
#define I2C_CLOCK_LO() I2C_DDR |=  ( 1 << I2C_CLK );

/*
 * Function to set data pin HIGH or LOW (depending on mode)
 */
void i2c_data(uint8_t mode){

}



void I2C_WriteBit( unsigned char c )
{
    if ( c > 0 )
    {
        I2C_DATA_HI();
    }
    else
    {
        I2C_DATA_LO();
    }

    I2C_CLOCK_HI();
    _delay_ms(1);

    I2C_CLOCK_LO();
    _delay_ms(1);

    if ( c > 0 )
    {
        I2C_DATA_LO();
    }

    _delay_ms(1);
   
}

unsigned char I2C_ReadBit()
{
    I2C_DATA_HI();

    I2C_CLOCK_HI();
    _delay_ms(1);

    unsigned char c = I2C_PIN;

    I2C_CLOCK_LO();
    _delay_ms(1);

    return ( c >> I2C_DAT ) & 1;
}

// Inits bitbanging port, must be called before using the functions below
//
void I2C_Init()
{
    I2C_PORT &= ~( ( 1 << I2C_DAT ) | ( 1 << I2C_CLK ) );

    I2C_CLOCK_HI();
    I2C_DATA_HI();

    _delay_ms(1);
}

// Send a START Condition
//
void I2C_Start()
{
    // set both to high at the same time
    I2C_DDR &=  ~( ( 1 << I2C_DAT ) | ( 1 << I2C_CLK ) );
    _delay_ms(1);
   
    I2C_DATA_LO();
    _delay_ms(1);

    I2C_CLOCK_LO();
    _delay_ms(1);
}

// Send a STOP Condition
//
void I2C_Stop()
{
    I2C_CLOCK_HI();
    _delay_ms(1);

    I2C_DATA_HI();
    _delay_ms(1);
}

// write a byte to the I2C slave device
//
unsigned char I2C_Write( unsigned char c )
{
    for ( char i=0;i<8;i++)
    {
        I2C_WriteBit( c & 128 );
   
     c<<=1;
    }

    return I2C_ReadBit();
}


// read a byte from the I2C slave device
//
unsigned char I2C_Read( unsigned char ack )
{
    unsigned char res = 0;

    for ( char i=0;i<8;i++)
    {
        res <<= 1;
        res |= I2C_ReadBit();  
    }

    if ( ack > 0)
    {
        I2C_WriteBit( 0 );
    }
    else
    {
        I2C_WriteBit( 1 );
    }

    _delay_ms(1);

    return res;
}
