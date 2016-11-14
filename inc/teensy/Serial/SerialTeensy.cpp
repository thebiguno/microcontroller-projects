#include "SerialTeensy.h"

#include <Arduino.h>

using namespace digitalcave;

SerialTeensy::SerialTeensy(uint32_t baud, uint8_t serialPort):
	serialPort(serialPort)
{
	if (serialPort == 0){
		Serial.begin(baud);
	}
	else if (serialPort == 1){
		Serial1.begin(baud);
	}
	else if (serialPort == 2){
		Serial2.begin(baud);
	}
	else if (serialPort == 3){
		Serial3.begin(baud);
	}
}

uint8_t SerialTeensy::read(uint8_t *c){
	if (serialPort == 0){
		if (Serial.available()){
			*c = Serial.read();
			return 1;
		}
	}
	else if (serialPort == 1){
		if (Serial1.available()){
			*c = Serial1.read();
			return 1;
		}
	}
	else if (serialPort == 2){
		if (Serial2.available()){
			*c = Serial2.read();
			return 1;
		}
	}
	else if (serialPort == 3){
		if (Serial3.available()){
			*c = Serial3.read();
			return 1;
		}
	}
	return 0;
}

uint8_t SerialTeensy::write(uint8_t b){
	if (serialPort == 0){
		Serial.write(b);
	}
	else if (serialPort == 1){
		Serial1.write(b);
	}
	else if (serialPort == 2){
		Serial2.write(b);
	}
	else if (serialPort == 3){
		Serial3.write(b);
	}
	return 1;
}
