#include <stdio.h>
#include "Protocol.h"

using namespace digitalcave;

Message::Message(uint8_t command){
	this->command = command;
	length = 0;
	committed = 0;
	escapedByte = 0;
}

Message::Message(uint8_t command, uint8_t* message, uint8_t length){
	this->command = command;
	this->length = length;
	for(uint8_t i = 0; i < length; i++){
		this->message[i] = message[i];
	}
	committed = 1;	//Do not allow any further modifications to this message
	position = 0;
	escapedByte = 0;
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
	if (committed == 0){
		message[length++] = b;
	}
}
uint8_t Message::escapeByte(uint8_t b){
	if (b == START || b == ESCAPE) {
		escapedByte = b ^ 0x20;
		return ESCAPE;
	} else {
		return b;
	}
}
		
uint8_t Message::flush(uint8_t* result){
	if (committed == 0){
		position = 0;
		committed = 1;
	}
	
	//If we have previously escaped a byte, we would have already sent the escape
	// character.  Now, the next time through flush(), we send the actual byte
	// xor'd with 0x20.
	if (escapedByte != 0){
		*result = escapedByte;
		escapedByte = 0;
		return 1;
	}
	
	switch(position){
		case 0:
			*result = START;
			position++;
			return 1;
		case 1:
			*result = escapeByte(length + 1);
			position++;
			return 1;
		case 2:
			*result = escapeByte(command);
			position++;
			return 1;
		default:
			if (position - 3 == length){
				//Write checksum
				*result = command;
				
				for (uint8_t i = 0; i < length; i++) {
					*result += message[i];
				}

				*result = escapeByte(0xff - *result);
				position++;
				return 1;
			}
			else if (position - 3 > length){
				return 0;	//No more to write
			}
			else {
				*result = escapeByte(message[position - 3]);
				position++;
				return 1;
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

Message* Protocol::poll(uint8_t b){
	if (error > 0 && b == START) {
		// recover from any previous error condition
		error = NO_ERROR;
		position = 0;
	} else if (error > 0) {
		return NULL;
	}
	
	if (position > 0 && b == START) {
		// unexpected start of frame
		error = INCOMING_ERROR_UNEXPECTED_START_OF_FRAME;
		return NULL;
	}
	if (position > 0 && b == ESCAPE) {
		// unescape next byte
		escape = 1;
		return NULL;
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
			return NULL;
		case 1: // length
			if (b == 0){
				error = INCOMING_ERROR_INVALID_LENGTH;
			}
			else {
				length = b;
				position++;
			}
			return NULL;
		case 2:
			command = b;
			position++;
			return NULL;
		default:
			if (position > MAX_SIZE){
				return NULL;
			}
			else if (position == (length + 2)) {
				if (checksum == 0xff) {
					return new Message(command, message, length - 1);
				} else {
					error = INCOMING_ERROR_INVALID_CHECKSUM;
				}
				position = 0;
				checksum = 0;
			}
			else {
				message[position++ - 3] = b;
			}
			return NULL;
	}
}
