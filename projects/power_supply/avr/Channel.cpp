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

	ADMUX = 0x00;		//Reference external AREF
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); 	//ADC Enable, prescaler = /128

	this->set_voltage_setpoint(this->voltage_startup);
	this->set_current_setpoint(this->current_startup);
}

/*
 * Voltage functions
 */
int16_t Channel::get_voltage_limit(){
	return this->voltage_limit;
}
int16_t Channel::get_voltage_setpoint(){
	return this->voltage_setpoint;
}

uint16_t Channel::get_voltage_setpoint_raw(){
	return this->voltage_setpoint_raw;
}

int16_t Channel::get_voltage_actual(){
	return get_adjusted_from_adc(this->voltage_actual_raw, this->calibration_voltage);
}


uint16_t Channel::get_voltage_actual_raw(){
	return this->voltage_actual_raw;
}

void Channel::set_voltage_setpoint(int16_t millivolts){
	if ((this->voltage_limit > 0 && millivolts > this->voltage_limit) || (this->voltage_limit < 0 && millivolts < this->voltage_limit)) millivolts = this->voltage_limit;
	if ((this->voltage_limit > 0 && millivolts < 0) || (this->voltage_limit < 0 && millivolts > 0)) millivolts = 0;

	this->voltage_setpoint = millivolts;
	this->set_voltage_setpoint_raw(get_dac_from_adjusted(millivolts, this->calibration_voltage));
}

void Channel::set_voltage_setpoint_raw(uint16_t raw_value){
	this->voltage_setpoint_raw = raw_value;
	if (raw_value > 0x0FFF) raw_value = 0x0FFF;
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER | this->dac_channel_voltage;		//Single write without EEPROM persist
	message[1] = 0x90 | ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.
	message[2] = (raw_value & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

int16_t Channel::get_voltage_startup(){
	return this->voltage_startup;
}
void Channel::set_voltage_startup(int16_t startup){
	this->voltage_startup = startup;
	this->save_calibration();

	//Set the power-on defaults to be the startup voltage
	uint16_t raw = get_dac_from_adjusted(startup, this->calibration_voltage);
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER_EEPROM | this->dac_channel_voltage;		//Single write with EEPROM persist
	message[1] = 0x90 | ((raw >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.
	message[2] = (raw & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}


/*
 * Current functions
 */
int16_t Channel::get_current_limit(){
	return this->current_limit;
}
int16_t Channel::get_current_setpoint(){
	return this->current_setpoint;
}

uint16_t Channel::get_current_setpoint_raw(){
	return this->current_setpoint_raw;
}

int16_t Channel::get_current_actual(){
	return get_adjusted_from_adc(this->current_actual_raw, this->calibration_current);
}


uint16_t Channel::get_current_actual_raw(){
	return this->current_actual_raw;
}

void Channel::set_current_setpoint(int16_t milliamps){
	if ((this->current_limit > 0 && milliamps > this->current_limit) || (this->current_limit < 0 && milliamps < this->current_limit)) milliamps = this->current_limit;
	if ((this->current_limit > 0 && milliamps < 0) || (this->current_limit < 0 && milliamps > 0)) milliamps = 0;

	this->current_setpoint = milliamps;
	this->set_current_setpoint_raw(get_dac_from_adjusted(milliamps, this->calibration_current));
}

void Channel::set_current_setpoint_raw(uint16_t raw_value){
	this->current_setpoint_raw = raw_value;
	if (raw_value > 0x0FFF) raw_value = 0x0FFF;
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER | this->dac_channel_current;		//Single write without EEPROM persist
	message[1] = 0x90 | ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.
	message[2] = (raw_value & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

int16_t Channel::get_current_startup(){
	return this->current_startup;
}
void Channel::set_current_startup(int16_t startup){
	this->current_startup = startup;
	this->save_calibration();

	//Set the power-on defaults to be the startup current
	uint16_t raw = get_dac_from_adjusted(startup, this->calibration_current);
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER_EEPROM | this->dac_channel_current;		//Single write with EEPROM persist
	message[1] = 0x90 | ((raw >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.
	message[2] = (raw & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

/*
 * ADC polling
 */

void Channel::sample_actual(){
	//*********** Sample voltage ***********//
	uint16_t sum = 0;

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

	for (uint8_t i = 0; i < 32; i++){
		ADCSRA |= _BV(ADSC);				//Start conversion
		while (!(ADCSRA & _BV(ADIF)));		//Wait until conversion is complete

		sum += ADC;

		_delay_us(1);
	}

	this->voltage_actual_raw = sum >> 5;	//Average of 32 samples


	//*********** Sample current ***********//
	sum = 0;

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

	for (uint8_t i = 0; i < 32; i++){
		ADCSRA |= _BV(ADSC);				//Start conversion
		while (!(ADCSRA & _BV(ADIF)));		//Wait until conversion is complete

		sum += ADC;

		_delay_us(1);
	}

	this->current_actual_raw = sum >> 5;	//Average of 32 samples
}

//Uses the calibration ADC value to give meaning to the raw adc value.  We assume a linear progression
// between calibration points.
int16_t Channel::get_adjusted_from_adc(uint16_t adc, calibration_t* calibration_data){
	calibration_t low = calibration_data[CALIBRATION_COUNT - 2];
	calibration_t high = calibration_data[CALIBRATION_COUNT - 1];

	//Find the two calibration points closest to the given adjusted value.  We try to find one point above
	// and one below.  If we happen to find an exact match, then just return it.  We assume that the
	// calibration data spans the entire calibration input range (i.e. there will never be a target
	// adjusted value which does not have both a calibration point above it and a calibration point
	// below it), and that the data set is in strictly increasing order.
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++){
		if (calibration_data[i].adc == adc) return calibration_data[i].adjusted;
		else if (calibration_data[i].adc > adc){
			if (i == 0) i++;	//Prevent index error if we are are at the bottom of the range
			low = calibration_data[i - 1];
			high = calibration_data[i];
			break;
		}
	}

	//At this point we should have a valid high and low value.  Find the slope + offset from the high / low
	// values, then return the interpolated adjusted value.
	double slope = ((double) high.adjusted - low.adjusted) / (high.adc - low.adc);
	return slope * ((double) adc - high.adc) + high.adjusted;
}
uint16_t Channel::get_dac_from_adjusted(int16_t adjusted, calibration_t* calibration_data){
	calibration_t low = calibration_data[CALIBRATION_COUNT - 2];
	calibration_t high = calibration_data[CALIBRATION_COUNT - 1];

	//Find the two calibration points closest to the given adjusted value.  We try to find one point above
	// and one below.  If we happen to find an exact match, then just return it.  We assume that the
	// calibration data spans the entire calibration input range (i.e. there will never be a target
	// adjusted value which does not have both a calibration point above it and a calibration point
	// below it), and that the data set is in strictly increasing order.
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++){
		if (calibration_data[i].adjusted == adjusted) return calibration_data[i].dac;
		else if (calibration_data[i].adjusted > adjusted){
			if (i == 0) i++;	//Prevent index error if we are are at the bottom of the range
			low = calibration_data[i - 1];
			high = calibration_data[i];
			break;
		}
	}

	//At this point we should have a valid high and low value.  Find the slope + offset from the high / low
	// values, then return the interpolated adjusted value.
	double slope = ((double) high.dac - low.dac) / (high.adjusted - low.adjusted);
	return slope * ((double) adjusted - high.adjusted) + high.dac;
}


/*
 * Persist / load calibration to / from EEPROM
 */
void Channel::save_calibration(){
	eeprom_update_block(this->calibration_voltage, (void*) (EEPROM_CALIBRATION_OFFSET + this->channel_index * 2 * EEPROM_CALIBRATION_BLOCK_SIZE), EEPROM_CALIBRATION_BLOCK_SIZE);
	eeprom_update_block(this->calibration_current, (void*) (EEPROM_CALIBRATION_OFFSET + this->channel_index * 2 * EEPROM_CALIBRATION_BLOCK_SIZE + EEPROM_CALIBRATION_BLOCK_SIZE), EEPROM_CALIBRATION_BLOCK_SIZE);

	eeprom_update_word((uint16_t*) (EEPROM_STARTUP_OFFSET + this->channel_index * 2 * EEPROM_STARTUP_BLOCK_SIZE), this->voltage_startup);
	eeprom_update_word((uint16_t*) (EEPROM_STARTUP_OFFSET + this->channel_index * 2 * EEPROM_STARTUP_BLOCK_SIZE + EEPROM_STARTUP_BLOCK_SIZE), this->current_startup);
}

void Channel::load_calibration(){
	eeprom_read_block(this->calibration_voltage, (void*) (EEPROM_CALIBRATION_OFFSET + this->channel_index * 2 * EEPROM_CALIBRATION_BLOCK_SIZE), EEPROM_CALIBRATION_BLOCK_SIZE);
	eeprom_read_block(this->calibration_current, (void*) (EEPROM_CALIBRATION_OFFSET + this->channel_index * 2 * EEPROM_CALIBRATION_BLOCK_SIZE + EEPROM_CALIBRATION_BLOCK_SIZE), EEPROM_CALIBRATION_BLOCK_SIZE);

	this->voltage_startup = eeprom_read_word((uint16_t*) (EEPROM_STARTUP_OFFSET + this->channel_index * 2 * EEPROM_STARTUP_BLOCK_SIZE));
	this->current_startup = eeprom_read_word((uint16_t*) (EEPROM_STARTUP_OFFSET + this->channel_index * 2 * EEPROM_STARTUP_BLOCK_SIZE + EEPROM_STARTUP_BLOCK_SIZE));
}


calibration_t Channel::get_calibration_voltage(uint8_t index){
	if (index > CALIBRATION_COUNT) index = CALIBRATION_COUNT - 1;
	return this->calibration_voltage[index];
}
calibration_t Channel::get_calibration_current(uint8_t index){
	if (index > CALIBRATION_COUNT) index = CALIBRATION_COUNT - 1;
	return this->calibration_current[index];
}
void Channel::set_calibration_voltage(uint8_t index, calibration_t calibration){
	if (index > CALIBRATION_COUNT) index = CALIBRATION_COUNT - 1;
	this->calibration_voltage[index] = calibration;
}
void Channel::set_calibration_current(uint8_t index, calibration_t calibration){
	if (index > CALIBRATION_COUNT) index = CALIBRATION_COUNT - 1;
	this->calibration_current[index] = calibration;
}
