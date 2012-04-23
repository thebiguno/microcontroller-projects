#include "mp3.h"

using namespace digitalcave;

MP3::MP3(volatile uint8_t *playPort, uint8_t playPin,
		volatile uint8_t *up, uint8_t upPin,
		volatile uint8_t *downPort, uint8_t downPin){
	this->playPort = playPort;
	this->playPin = playPin;
	this->upPort = upPort;
	this->upPin = upPin;
	this->downPort = downPort;
	this->downPin = downPin;
	
	*playPort |= _BV(playPin);
	*(playPort - 0x01) |= _BV(playPin);
	*upPort |= _BV(upPin);
	*(upPort - 0x01) |= _BV(upPin);
	*downPort |= _BV(downPin);
	*(downPort - 0x01) |= _BV(downPin);
	
	for (uint8_t i = 0; i < 10; i++){
		this->hitVolumeDown();
	}
	
	for (uint8_t i = 0; i < 5; i++){
		this->hitVolumeUp();
	}	
}

static void hitButton(volatile uint8_t *port, uint8_t pin){
	*port &= ~_BV(pin);
	_delay_ms(80);
	*port |= _BV(pin);
	_delay_ms(80);	
}
	

void MP3::hitPlayPause(){
	hitButton(this->playPort, this->playPin);
}

void MP3::hitVolumeUp(){
	hitButton(this->upPort, this->upPin);
}

void MP3::hitVolumeDown(){
	hitButton(this->downPort, this->downPin);
}
