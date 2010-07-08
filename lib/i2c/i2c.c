/* Start an I2C transmission to slave_address, of the given mode.  Mode should be 
 * one of TW_WRITE (logic 0) or TW_READ (logic 1).
 */
uint8_t i2c_start(uint8_t slave_address, uint8_t mode){
	//Send START condition
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

	//Wait for START condition to be sent
	while (!(TWCR & _BV(TWINT)))
		; 

	//Check value of TWI Status Register. Mask prescaler bits. If status different from START return ERROR
	if ((TWSR & 0xF8) != START)
		return -1;

	//Load slave address + write into TWDR Register. Clear TWINT bit in TWCR to start transmission of address
	TWDR = (slave_address << 1) | (mode & 0x1);;
	TWCR = _BV(TWINT) | _BV(TWEN);

	//Wait for TWINT Flag set. This indicates that the SLA+W has been transmitted, and ACK/NACK has been received.
	while (!(TWCR & _BV(TWINT))) 
		;

	//Check value of TWI Status Register. Mask prescaler bits. If status different from MT_SLA_ACK return error.
	if ((TWSR & 0xF8) != MT_SLA_ACK)
		return -2;
}

/* Write the I2C data.  You must call i2c_start() before this to address a certain slave.
 */
uint8_t i2c_write(uint8_t data){
	//Load DATA into TWDR Register. Clear TWINT bit in TWCR to start transmission of data
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);

	//Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
	while (!(TWCR & _BV(TWINT)))
		;

	//Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK return error
	if ((TWSR & 0xF8) != MT_DATA_ACK)
		return -1;

}

uint8_t i2c_stop(){
	//Transmit STOP condition
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}
