#include "Channel.h"

using namespace digitalcave;

Channel::Channel(uint8_t i2c_address, 
					double voltage_max, double voltage_min, uint8_t voltage_adc_channel, 
					double current_max, double current_min, uint8_t current_adc_channel){
	this->i2c_address = i2c_address;
	this->voltage_max = voltage_max;
	this->voltage_min = voltage_min;
	this->voltage_adc_channel = voltage_adc_channel;
	
	this->current_max = current_max;
	this->current_min = current_min;
	this->current_adc_channel = current_adc_channel;
}

double Channel::get_voltage_setpoint(){
	return this->voltage_setpoint;
}
void Channel::set_voltage_setpoint(double setpoint){
	this->voltage_setpoint = setpoint;
}

double Channel::get_current_setpoint(){
	return this->current_setpoint;
}
void Channel::set_current_setpoint(double setpoint){
	this->voltage_setpoint = setpoint;
}

void Channel::sample_actual(){
	//TODO
	this->voltage_actual = 1.234 + this->i2c_address;
	this->current_actual = 4.321 + this->i2c_address;
}

void Channel::to_string(uint8_t actual, char* buffer, uint8_t length){
	double voltage = actual ? this->voltage_actual : this->voltage_setpoint;
	double current = actual ? this->current_actual : this->current_setpoint;
	snprintf(buffer, length, "%d %+6.2fV %5.3fA     ", this->i2c_address + 1, voltage, current);
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
