#include <stdio.h>
#include "Protocol.h"

using namespace digitalcave;

Message::Message(uint8_t command){
	this->command = command;
	length = 0;
}

Message::Message(uint8_t command, uint8_t* message, uint8_t length){
	this->command = command;
	this->length = length;
	for(uint8_t i = 0; i < length; i++){
		this->message[i] = message[i];
	}
}

void Message::clone(Message* m){
	command = m->command;
	for(uint8_t i = 0; i < MAX_SIZE; i++){
		message[i] = m->message[i];
	}
	length = m->length;
}

uint8_t Message::getCommand(){
	return command;
}

uint8_t Message::getLength(){
	return length;
}

uint8_t* Message::getMessage(){
	return message;
}

void Message::append(uint8_t b){
	message[length++] = b;
}
void Message::escapeByte(Stream* stream, uint8_t b){
	if (b == START || b == ESCAPE) {
		stream->write(ESCAPE);
		stream->write(b ^ 0x20);
	} else {
		stream->write(b);
	}
}
		
void Message::write(Stream* stream){
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
				break;
			default:
				if (position - 3 == length){
					//Write checksum
					uint8_t result = command;
				
					for (uint8_t i = 0; i < length; i++) {
						result += message[i];
					}

					escapeByte(stream, 0xff - result);
				}
				else {
					escapeByte(stream, message[position - 3]);
				}
				break;
		}
	}
}

Protocol::Protocol(){
	position = 0;
	length = 0;
	command = 0;
	checksum = 0;
	escape = 0;
	error = 0;
}

uint8_t Protocol::getError(){
	return error;
}

uint8_t Protocol::read(Stream* stream, Message* result){
	uint8_t b;
	while (stream->read(&b)){
		if (error > 0){
			if (b == START) {
				// recover from any previous error condition
				error = NO_ERROR;
				position = 0;
			}
			continue;
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
				if (position > MAX_SIZE){
					//Max size exceeded
					error = INCOMING_ERROR_EXCEED_MAX_LENGTH;
				}
				else if (position == (length + 2)) {
					if (checksum == 0xff) {
						Message m(command, message, length - 1);
						result->clone(&m);
						return 1;
					} else {
						error = INCOMING_ERROR_INVALID_CHECKSUM;
					}
					position = 0;
					checksum = 0;
				}
				else {
					message[position++ - 3] = b;
				}
				break;
		}
	}
	
	return 0;
}
