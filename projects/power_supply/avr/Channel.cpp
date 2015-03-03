#include "Channel.h"

using namespace digitalcave;

Channel::Channel(uint8_t i2c_address,
					uint8_t bank,
					double voltage_min, double voltage_max, uint8_t voltage_adc_channel, 
					double current_min, double current_max, uint8_t current_adc_channel){
	this->i2c_address = i2c_address;
	this->bank = bank;
	this->voltage_max = voltage_max;
	this->voltage_min = voltage_min;
	this->voltage_adc_channel = voltage_adc_channel;
	
	this->current_max = current_max;
	this->current_min = current_min;
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

double Channel::get_voltage_setpoint(){
	return this->voltage_setpoint;
}
void Channel::set_voltage_setpoint(double setpoint){
	this->voltage_setpoint = setpoint;

	uint16_t dac_setpoint = ((uint16_t) (((setpoint / VOLTAGE_MULTIPLIER) / 5) * 0x1000)) & 0x0FFF;
	uint8_t message[3];
	message[0] = 0x58 | (this->bank << 2);		//Single write to channel 0 or 2 (depending on bank) (see datasheet page 41)
	message[1] = ((dac_setpoint >> 8) & 0x0F);	//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
	message[2] = (dac_setpoint & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

double Channel::get_current_setpoint(){
	return this->current_setpoint;
}
void Channel::set_current_setpoint(double setpoint){
	this->current_setpoint = setpoint;
	
	uint16_t dac_setpoint = ((uint16_t) (((setpoint / CURRENT_MULTIPLIER) / 5) * 0x1000)) & 0x0FFF;
	uint8_t message[3];
	message[0] = 0x58 | (this->bank << 2) | (1 << 1);	//Single write to channel 0 or 2 (depending on bank) (see datasheet page 41)
	message[1] = ((dac_setpoint >> 8) & 0x0F);			//First nibble is [VREF,PD1,PD0,Gx].  Set all of these to zero.
	message[2] = (dac_setpoint & 0xFF);
	twi_write_to(this->i2c_address, message, 3, TWI_BLOCK, TWI_STOP);
}

void Channel::sample_actual(){
	//Set up which pin to read for voltage
	ADMUX &= ~0x1F;
	if (this->voltage_adc_channel <= 0x07){
		ADMUX |= this->voltage_adc_channel;
	}
	else {
		ADMUX |= this->voltage_adc_channel + 0x18;
	}
	ADCSRA |= _BV(ADSC);			//Start conversion
	while (ADCSRA & _BV(ADSC));		//Wait until conversion is complete

	this->voltage_actual = ((double) ADC / 1024) * 5 * VOLTAGE_MULTIPLIER;
	
	
	//Set up which pin to read for current
	ADMUX &= ~0x1F;
	if (this->voltage_adc_channel <= 0x07){
		ADMUX |= this->voltage_adc_channel;
	}
	else {
		ADMUX |= this->voltage_adc_channel + 0x18;
	}
	ADCSRA |= _BV(ADSC);			//Start conversion
	while (ADCSRA & _BV(ADSC));		//Wait until conversion is complete

	this->current_actual = ((double) ADC / 1024) * 5 * CURRENT_MULTIPLIER;;
}

void Channel::to_string(uint8_t index, uint8_t actual, char* buffer, uint8_t max_length){
	double voltage = actual ? this->voltage_actual : this->voltage_setpoint;
	double current = actual ? this->current_actual : this->current_setpoint;
	snprintf(buffer, max_length, "%d %+6.2fV %5.3fA     ", index + 1, voltage, current);
}

void Channel::adjust_setpoint(uint8_t selector, double amount){
	if (selector == 0){
		this->voltage_setpoint += amount;
		if (this->voltage_setpoint >= this->voltage_max) this->voltage_setpoint = this->voltage_max;
		else if (this->voltage_setpoint <= this->voltage_min) this->voltage_setpoint = this->voltage_min;
	}
	else {
		this->current_setpoint += amount;
		if (this->current_setpoint >= this->current_max) this->current_setpoint = this->current_max;
		else if (this->current_setpoint <= this->current_min) this->current_setpoint = this->current_min;
	}
}
