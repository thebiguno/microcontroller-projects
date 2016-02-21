#include "FramedSerialProtocol.h"

using namespace digitalcave;

FramedSerialMessage::FramedSerialMessage(uint8_t command, uint8_t maxSize){
	this->data = (uint8_t*) malloc(maxSize);
	this->command = command;
	this->maxSize = maxSize;
	length = 0;
}

FramedSerialMessage::~FramedSerialMessage(){
	free((void*) data);
}

FramedSerialMessage::FramedSerialMessage(uint8_t command, uint8_t* data, uint8_t length){
	this->data = (uint8_t*) malloc(maxSize);
	this->command = command;
	this->length = length;
	this->maxSize = length;
	for(uint8_t i = 0; i < length; i++){
		this->data[i] = data[i];
	}
}

void FramedSerialMessage::clone(FramedSerialMessage* m){
	command = m->command;
	for(uint8_t i = 0; i < maxSize; i++){
		data[i] = m->data[i];
	}
	length = m->length;
}

uint8_t FramedSerialMessage::getCommand(){
	return command;
}

uint8_t FramedSerialMessage::getLength(){
	return length;
}

uint8_t* FramedSerialMessage::getData(){
	return data;
}

void FramedSerialMessage::append(uint8_t b){
	data[length++] = b;
}

FramedSerialProtocol::FramedSerialProtocol(uint8_t maxSize){
	data = (uint8_t*) malloc(maxSize);
	this->maxSize = maxSize;
	position = 0;
	length = 0;
	command = 0;
	checksum = 0;
	escape = 0;
	error = 0;
}

FramedSerialProtocol::~FramedSerialProtocol(){
	free(data);
}

uint8_t FramedSerialProtocol::getError(){
	return error;
}

void FramedSerialProtocol::escapeByte(Stream* stream, uint8_t b){
	if (b == START || b == ESCAPE) {
		stream->write(ESCAPE);
		stream->write(b ^ 0x20);
	} else {
		stream->write(b);
	}
}

uint8_t FramedSerialProtocol::read(Stream* stream, FramedSerialMessage* result){
	uint8_t b;
	while (stream->read(&b)){
		if (position == 0 && b != START){
			//Garbage data, ignore
			continue;
		}
		
		if (error > 0){
			if (b == START) {
				// recover from any previous error condition
				error = NO_ERROR;
				position = 0;
			}
			else {
				continue;
			}
		}
	
		if (position > 0 && b == START) {
			// unexpected start of frame
			error = INCOMING_ERROR_UNEXPECTED_START_OF_FRAME;
			continue;
		}
		if (position > 0 && b == ESCAPE) {
			// unescape next byte
			escape = 1;
			continue;
		}
		if (escape) {
			// unescape current byte
			b = 0x20 ^ b;
			escape = 0;
		}
		if (position > 1) { // start byte and length byte not included in checksum
			checksum += b;
		}
	
		switch(position) {
			case 0: // start frame
				position++;
				break;
			case 1: // length
				if (b == 0){
					error = INCOMING_ERROR_INVALID_LENGTH;
				}
				else {
					length = b;
					position++;
				}
				break;
			case 2:
				command = b;
				position++;
				break;
			default:
				if ((position - 3) > maxSize){
					//Max size exceeded
					error = INCOMING_ERROR_EXCEED_MAX_LENGTH;
				}
				else if (position == (length + 2)) {
					if (checksum == 0xff) {
						FramedSerialMessage m(command, data, length - 1);
						result->clone(&m);
						position = 0;
						checksum = 0;
						return 1;
					} else {
						error = INCOMING_ERROR_INVALID_CHECKSUM;
					}
					position = 0;
					checksum = 0;
				}
				else {
					data[position - 3] = b;
					position++;
				}
				break;
		}
	}
	
	return 0;
}

void FramedSerialProtocol::write(Stream* stream, FramedSerialMessage* message){
	uint8_t length = message->getLength();
	uint8_t command = message->getCommand();
	uint8_t* data = message->getData();
	
	uint8_t checksum  = 0;
	for(uint8_t position = 0; position <= (length + 3); position++){
		switch(position){
			case 0:
				stream->write(START);
				break;
			case 1:
				escapeByte(stream, length + 1);
				break;
			case 2:
				escapeByte(stream, command);
				checksum = command;
				break;
			default:
				if (position - 3 == length){
					escapeByte(stream, 0xff - checksum);
				}
				else {
					uint8_t b = data[position - 3];
					escapeByte(stream, b);
					checksum += b;
				}
				break;
		}
	}
}

