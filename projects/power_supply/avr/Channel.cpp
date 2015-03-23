#include "Channel.h"

using namespace digitalcave;

Channel::Channel(uint8_t channel_index, uint8_t i2c_address, uint8_t dac_channel_voltage, uint8_t dac_channel_current,
		uint8_t adc_channel_voltage, uint8_t adc_channel_current,
		int16_t voltage_limit, int16_t current_limit){

	this->channel_index = channel_index;
	
	this->i2c_address = i2c_address;
	this->dac_channel_voltage = dac_channel_voltage;
	this->dac_channel_current = dac_channel_current;

	this->adc_channel_voltage = adc_channel_voltage;
	this->adc_channel_current = adc_channel_current;

	this->voltage_limit = voltage_limit;
	this->current_limit = current_limit;
	
	this->load_calibration();
	
	twi_init();
	
	//Analog setup
	//Disable digital inputs
	DIDR0 = 0xFF;
	DIDR2 = 0xFF;
	
	ADMUX = _BV(REFS0);		//Reference AVCC with cap at AREF pin
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); 	//ADC Enable, prescaler = /128
	
	this->set_voltage_setpoint(0);
	this->set_current_setpoint(0);
}

/*
 * Voltage functions
 */

int16_t Channel::get_voltage_setpoint(){
	//scaled_value = slope * raw_value + offset
	return this->voltage_setpoint_slope * this->voltage_setpoint_raw + this->voltage_setpoint_offset;
}


uint16_t Channel::get_voltage_setpoint_raw(){
	return this->voltage_setpoint_raw;
}

int16_t Channel::get_voltage_actual(){
	//scaled_value = slope * raw_value + offset
	return this->voltage_actual_slope * this->voltage_actual_raw + this->voltage_actual_offset;
}


uint16_t Channel::get_voltage_actual_raw(){
	return this->voltage_actual_raw;
}

void Channel::set_voltage_setpoint(int16_t millivolts){
	//Since "scaled_value = slope * raw_value + offset", we know
	// that "raw_value = (scaled_value - offset) / slope".
	if ((this->voltage_limit > 0 && millivolts > this->voltage_limit) || (this->voltage_limit < 0 && millivolts < this->voltage_limit)) millivolts = this->voltage_limit;
	if ((this->voltage_limit > 0 && millivolts < 0) || (this->voltage_limit < 0 && millivolts > 0)) millivolts = 0;
	double raw_value = ((double) millivolts - this->voltage_setpoint_offset) / this->voltage_setpoint_slope;
	if (raw_value < 0) raw_value = 0;
	this->set_voltage_setpoint_raw(raw_value);
}

void Channel::set_voltage_setpoint_raw(uint16_t raw_value){
	this->voltage_setpoint_raw = raw_value;
	if (raw_value > 0x0FFF) raw_value = 0x0FFF;
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER | this->dac_channel_voltage;		//Single write without EEPROM persist
	message[1] = ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
	message[2] = (raw_value & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

/*
 * Current functions
 */

int16_t Channel::get_current_setpoint(){
	//scaled_value = slope * raw_value + offset
	return this->current_setpoint_slope * this->current_setpoint_raw + this->current_setpoint_offset;
}

uint16_t Channel::get_current_setpoint_raw(){
	return this->current_setpoint_raw;
}

int16_t Channel::get_current_actual(){
	//scaled_value = slope * raw_value + offset
	return this->current_actual_slope * this->current_actual_raw + this->current_actual_offset;
}


uint16_t Channel::get_current_actual_raw(){
	return this->current_actual_raw;
}

void Channel::set_current_setpoint(int16_t milliamps){
	//Since "scaled_value = slope * raw_value + offset", we know
	// that "raw_value = (scaled_value - offset) / slope".
	if (milliamps > (int16_t) this->current_limit) milliamps = this->current_limit;
	if (milliamps <= 0) milliamps = 0;
	double raw_value = ((double) milliamps - this->current_setpoint_offset) / this->current_setpoint_slope;
	if (raw_value < 0) raw_value = 0;
	this->set_current_setpoint_raw(raw_value);
}

void Channel::set_current_setpoint_raw(uint16_t raw_value){
	this->current_setpoint_raw = raw_value;
	if (raw_value > 0x0FFF) raw_value = 0x0FFF;
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER | this->dac_channel_current;		//Single write without EEPROM persist
	message[1] = ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
	message[2] = (raw_value & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}


/*
 * ADC polling
 */
 
void Channel::sample_actual(){
	//Set up which pin to read for voltage
	ADCSRA &= ~_BV(ADIF);
	ADCSRB &= ~_BV(MUX5);
	ADMUX &= ~(_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
	if (this->adc_channel_voltage <= 0x07){
		ADMUX |= this->adc_channel_voltage;
	}
	else {
		ADMUX |= this->adc_channel_voltage - 0x08;
		ADCSRB |= _BV(MUX5);
	}
	ADCSRA |= _BV(ADSC);				//Start conversion
	while (!(ADCSRA & _BV(ADIF)));		//Wait until conversion is complete

	this->voltage_actual_raw = ADC;
	
	_delay_us(1);
	
	//Set up which pin to read for current
	ADCSRA &= ~_BV(ADIF);
	ADCSRB &= ~_BV(MUX5);
	ADMUX &= ~(_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
	
	if (this->adc_channel_current <= 0x07){
		ADMUX |= this->adc_channel_current;
	}
	else {
		ADMUX |= this->adc_channel_current - 0x08;
		ADCSRB |= _BV(MUX5);
	}
	ADCSRA |= _BV(ADSC);				//Start conversion
	while (!(ADCSRA & _BV(ADIF)));		//Wait until conversion is complete

	this->current_actual_raw = ADC;
	
	_delay_us(1);
}

/*
	* Persist / load calibration to / from EEPROM
	*/
void Channel::save_calibration(){
	//Each channel needs 8 * 4 = 32 bytes of EEPROM.
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 0 * sizeof(float)), (float) this->voltage_actual_slope);
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 1 * sizeof(float)), (float) this->voltage_actual_offset);
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 2 * sizeof(float)), (float) this->voltage_setpoint_slope);
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 3 * sizeof(float)), (float) this->voltage_setpoint_offset);
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 4 * sizeof(float)), (float) this->current_actual_slope);
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 5 * sizeof(float)), (float) this->current_actual_offset);
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 6 * sizeof(float)), (float) this->current_setpoint_slope);
	eeprom_update_float((float*) (this->channel_index * 8 * sizeof(float) + 7 * sizeof(float)), (float) this->current_setpoint_offset);
	
	double raw_value_double = (0 - this->voltage_setpoint_offset) / this->voltage_setpoint_slope;
	if (raw_value_double < 0) raw_value_double = 0;
	uint16_t raw_value = raw_value_double;
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER_EEPROM | this->dac_channel_voltage;		//Single write with EEPROM persist
	message[1] = ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
	message[2] = (raw_value & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);

 	raw_value_double = (0 - this->current_setpoint_offset) / this->current_setpoint_slope;
	if (raw_value_double < 0) raw_value_double = 0;
	raw_value = raw_value_double;
 	message[0] = DAC_COMMAND_REGISTER_EEPROM | this->dac_channel_current;		//Single write with EEPROM persist
 	message[1] = ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
 	message[2] = (raw_value & 0xFF);
 	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

void Channel::load_calibration(){
	this->voltage_actual_slope = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 0 * sizeof(float)));
	this->voltage_actual_offset = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 1 * sizeof(float)));
	this->voltage_setpoint_slope = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 2 * sizeof(float)));
	this->voltage_setpoint_offset = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 3 * sizeof(float)));
	this->current_actual_slope = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 4 * sizeof(float)));
	this->current_actual_offset = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 5 * sizeof(float)));
	this->current_setpoint_slope = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 6 * sizeof(float)));
	this->current_setpoint_offset = eeprom_read_float((float*) (this->channel_index * 8 * sizeof(float) + 7 * sizeof(float)));
}