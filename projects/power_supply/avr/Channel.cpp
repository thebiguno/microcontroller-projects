#include "Channel.h"

using namespace digitalcave;

Channel::Channel(uint8_t i2c_address, uint8_t dac_bank,
					int16_t voltage_limit, int16_t current_limit,
					uint8_t voltage_adc_channel, uint8_t current_adc_channel){
	this->i2c_address = i2c_address;
	this->dac_bank = dac_bank;
	this->voltage_limit = voltage_limit;
	this->voltage_adc_channel = voltage_adc_channel;
	
	this->current_limit = current_limit;
	this->current_adc_channel = current_adc_channel;
	
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

int16_t Channel::get_voltage_actual(){
	return this->voltage_actual;
}
int16_t Channel::get_voltage_setpoint(){
	return this->voltage_setpoint;
}
void Channel::set_voltage_setpoint(int16_t setpoint){
	this->voltage_setpoint = setpoint;

	uint16_t dac_setpoint = ((uint16_t) (((setpoint / 1000.0 / VOLTAGE_MULTIPLIER) / 5) * 0x1000)) & 0x0FFF;
	uint8_t message[3];
	message[0] = 0x58 | (this->dac_bank << 2);		//Single write to channel 0 or 2 (depending on bank) (see datasheet page 41)
	message[1] = ((dac_setpoint >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
	message[2] = (dac_setpoint & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

int16_t Channel::get_current_actual(){
	return this->current_actual;
}
int16_t Channel::get_current_setpoint(){
	return this->current_setpoint;
}
void Channel::set_current_setpoint(int16_t setpoint){
	this->current_setpoint = setpoint;
	
	uint16_t dac_setpoint = ((uint16_t) (((setpoint / 1000.0 / CURRENT_MULTIPLIER) / 5) * 0x1000)) & 0x0FFF;
	uint8_t message[3];
	message[0] = 0x58 | (this->dac_bank << 2) | (0x02);	//Single write to channel 0 or 2 (depending on bank) (see datasheet page 41)
	message[1] = ((dac_setpoint >> 8) & 0x0F);			//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
	message[2] = (dac_setpoint & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

void Channel::sample_actual(){
	//Set up which pin to read for voltage
	ADCSRA &= ~_BV(ADIF);
	ADCSRB &= ~_BV(MUX5);
	ADMUX &= ~(_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
	if (this->voltage_adc_channel <= 0x07){
		ADMUX |= this->voltage_adc_channel;
	}
	else {
		ADMUX |= this->voltage_adc_channel - 0x08;
		ADCSRB |= _BV(MUX5);
	}
	ADCSRA |= _BV(ADSC);				//Start conversion
	while (!(ADCSRA & _BV(ADIF)));		//Wait until conversion is complete

	double sample;
	sample = ADC;
	sample = (sample / 1024.0) * 5000 * VOLTAGE_MULTIPLIER;
	this->voltage_actual = (sample + ((RUNNING_AVERAGE_COUNT_VOLTAGE - 1) * (double) this->voltage_actual)) / RUNNING_AVERAGE_COUNT_VOLTAGE;
	
	_delay_us(1);
	
	//Set up which pin to read for current
	ADCSRA &= ~_BV(ADIF);
	ADCSRB &= ~_BV(MUX5);
	ADMUX &= ~(_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
	
	if (this->current_adc_channel <= 0x07){
		ADMUX |= this->current_adc_channel;
	}
	else {
		ADMUX |= this->current_adc_channel - 0x08;
		ADCSRB |= _BV(MUX5);
	}
	ADCSRA |= _BV(ADSC);				//Start conversion
	while (!(ADCSRA & _BV(ADIF)));		//Wait until conversion is complete

	sample = ADC;
	sample = (sample / 1024.0) * 5000 * CURRENT_MULTIPLIER;
	this->current_actual = (sample + ((RUNNING_AVERAGE_COUNT_CURRENT - 1) * (double) this->current_actual)) / RUNNING_AVERAGE_COUNT_CURRENT;
	
	_delay_us(1);
}

void Channel::adjust_setpoint(uint8_t selector, int16_t amount){
	if (selector == 0){
		this->voltage_setpoint += amount;
		
		if (this->voltage_limit >= 0){
			if (this->voltage_setpoint >= this->voltage_limit) this->voltage_setpoint = this->voltage_limit;
			else if (this->voltage_setpoint <= 0) this->voltage_setpoint = 0;
		}
		else {
			if (this->voltage_setpoint < 0 && this->voltage_setpoint <= this->voltage_limit) this->voltage_setpoint = this->voltage_limit;
			else if (this->voltage_setpoint <= 0) this->voltage_setpoint = 0;
		}
		
		this->set_voltage_setpoint(this->voltage_setpoint);
	}
	else {
		this->current_setpoint += amount;
		
		if (this->current_setpoint >= this->current_limit) this->current_setpoint = this->current_limit;
		else if (this->current_setpoint <= 0) this->current_setpoint = 0;
		
		this->set_current_setpoint(this->current_setpoint);
	}
}
