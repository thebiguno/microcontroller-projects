#include "Channel.h"

//#include <Serial/SerialUSB.h>

using namespace digitalcave;

//extern SerialUSB serial;

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
	return get_actual_from_adc(this->voltage_actual_raw, this->calibration_voltage);
}


uint16_t Channel::get_voltage_actual_raw(){
	return this->voltage_actual_raw;
}

void Channel::set_voltage_setpoint(int16_t millivolts){
	// char buffer[128];
	// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_voltage_setpoint: %dmV\n\r", millivolts));
	if ((this->voltage_limit > 0 && millivolts > this->voltage_limit) || (this->voltage_limit < 0 && millivolts < this->voltage_limit)){
		millivolts = this->voltage_limit;
		// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_voltage_setpoint over limit, adjusting to : %dmV\n\r", millivolts));
	}
	if ((this->voltage_limit > 0 && millivolts < 0) || (this->voltage_limit < 0 && millivolts > 0)){
		millivolts = 0;
		// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_voltage_setpoint under limit, adjusting to : %dmV\n\r", millivolts));
	}

	this->voltage_setpoint = millivolts;
	this->set_voltage_setpoint_raw(get_dac_from_actual(millivolts, this->calibration_voltage));
}

void Channel::set_voltage_setpoint_raw(uint16_t raw_value){
	this->voltage_setpoint_raw = raw_value;
	if (raw_value > 0x0FFF) raw_value = 0x0FFF;
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER | this->dac_channel_voltage;		//Single write without EEPROM persist
	message[1] = 0x90 | ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.
	message[2] = (raw_value & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);

	// char buffer[128];
	// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_voltage_setpoint_raw: %d\n\r", raw_value));
}

int16_t Channel::get_voltage_startup(){
	return this->voltage_startup;
}
void Channel::set_voltage_startup(int16_t startup){
	this->voltage_startup = startup;
	this->save_calibration();

	//Set the power-on defaults to be the startup voltage
	uint16_t raw = get_dac_from_actual(startup, this->calibration_voltage);
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
	return get_actual_from_adc(this->current_actual_raw, this->calibration_current);
}


uint16_t Channel::get_current_actual_raw(){
	return this->current_actual_raw;
}

void Channel::set_current_setpoint(int16_t milliamps){
	// char buffer[128];
	// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_current_setpoint: %dmA\n\r", milliamps));

	if ((this->current_limit > 0 && milliamps > this->current_limit) || (this->current_limit < 0 && milliamps < this->current_limit)){
		milliamps = this->current_limit;
		// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_current_setpoint over limit, adjusting to : %dmA\n\r", milliamps));
	}
	if ((this->current_limit > 0 && milliamps < 0) || (this->current_limit < 0 && milliamps > 0)){
		milliamps = 0;
		// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_current_setpoint under limit, adjusting to : %dmA\n\r", milliamps));
	}

	this->current_setpoint = milliamps;
	this->set_current_setpoint_raw(get_dac_from_actual(milliamps, this->calibration_current));
}

void Channel::set_current_setpoint_raw(uint16_t raw_value){
	this->current_setpoint_raw = raw_value;
	if (raw_value > 0x0FFF) raw_value = 0x0FFF;
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER | this->dac_channel_current;		//Single write without EEPROM persist
	message[1] = 0x90 | ((raw_value >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.
	message[2] = (raw_value & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);

	// char buffer[128];
	// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "set_current_setpoint_raw: %d\n\r", raw_value));
}

int16_t Channel::get_current_startup(){
	return this->current_startup;
}
void Channel::set_current_startup(int16_t startup){
	this->current_startup = startup;
	this->save_calibration();

	//Set the power-on defaults to be the startup current
	uint16_t raw = get_dac_from_actual(startup, this->calibration_current);
	uint8_t message[3];
	message[0] = DAC_COMMAND_REGISTER_EEPROM | this->dac_channel_current;		//Single write with EEPROM persist
	message[1] = 0x90 | ((raw >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.
	message[2] = (raw & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

/*
 * Retrieve the latest raw ADC value from the async analog library
 */
void Channel::sample_actual(){
	this->voltage_actual_raw = analog_read_p(this->channel_index * 2);
	this->current_actual_raw = analog_read_p((this->channel_index * 2) + 1);

	// char buffer[128];
	// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Channel %d ADC: %d, %d\n\r", this->channel_index, this->voltage_actual_raw, this->current_actual_raw));
}

//Uses the calibration ADC value to give meaning to the raw adc value.  We assume a linear progression
// between calibration points.
int16_t Channel::get_actual_from_adc(uint16_t adc, calibration_t* calibration_data){
	calibration_t low = calibration_data[CALIBRATION_COUNT - 2];
	calibration_t high = calibration_data[CALIBRATION_COUNT - 1];

	//Find the two calibration points closest to the given actual value.  We try to find one point above
	// and one below.  If we happen to find an exact match, then just return it.  We assume that the
	// calibration data spans the entire calibration input range (i.e. there will never be a target
	// actual value which does not have both a calibration point above it and a calibration point
	// below it), and that the data set is in strictly increasing order.
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++){
		if (calibration_data[i].adc == adc) return calibration_data[i].actual;
		else if (calibration_data[i].adc > adc){
			if (i == 0) i++;	//Prevent index error if we are are at the bottom of the range
			low = calibration_data[i - 1];
			high = calibration_data[i];
			break;
		}
	}

	//At this point we should have a valid high and low value.  Find the slope + offset from the high / low
	// values, then return the interpolated actual value.
	double slope = ((double) high.actual - low.actual) / (high.adc - low.adc);
	return slope * ((double) adc - high.adc) + high.actual;
}
uint16_t Channel::get_dac_from_actual(int16_t actual, calibration_t* calibration_data){
	actual = abs(actual);	//We only look at the absolute value here, so that negative channels are handled properly.
	calibration_t low = calibration_data[CALIBRATION_COUNT - 2];
	calibration_t high = calibration_data[CALIBRATION_COUNT - 1];

	//Find the two calibration points closest to the given actual value.  We try to find one point above
	// and one below.  If we happen to find an exact match, then just return it.  We assume that the
	// calibration data spans the entire calibration input range (i.e. there will never be a target
	// actual value which does not have both a calibration point above it and a calibration point
	// below it), and that the data set is in strictly increasing order.
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++){
		if (abs(calibration_data[i].actual) == actual) return calibration_data[i].dac;
		else if (abs(calibration_data[i].actual) > actual){
			if (i == 0){
				i++;	//Prevent index error if we are are at the bottom of the range
			}
			low = calibration_data[i - 1];
			high = calibration_data[i];
			break;
		}
	}

	//At this point we should have a valid high and low value.  Find the slope + offset from the high / low
	// values, then return the interpolated actual value.
	double slope = ((double) high.dac - low.dac) / (abs(high.actual) - abs(low.actual));
	return slope * ((double) actual - abs(high.actual)) + high.dac;
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
