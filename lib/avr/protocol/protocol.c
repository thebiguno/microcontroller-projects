#include "protocol.h"

#define START 0x7e
#define ESCAPE 0x7d

#define MAX_SIZE 40

static uint8_t position;			// current position in the frame
static uint8_t length;				// frame length
static uint8_t command;				// frame api code (command)
static uint8_t checksum;			// checksum
static uint8_t escape;	 			// escape byte seen, unescape next byte
static uint8_t error;	 			// error condition, ignore bytes until next frame start byte
static uint8_t message[MAX_SIZE];

void _protocol_send_byte(uint8_t b, uint8_t escape)
{
	if (escape && (b == START || b == ESCAPE)) {
		protocol_write(ESCAPE);
		protocol_write(b ^ 0x20);
	} else {
		protocol_write(b);
	}
}

void protocol_send_message(uint8_t cmd, uint8_t *bytes, uint8_t length)
{
	_protocol_send_byte(START, 0);
	_protocol_send_byte(length + 1, 1);
	_protocol_send_byte(cmd, 0);
	
	uint8_t checksum = cmd;

	for (uint8_t i = 0; i < length; i++) {
		_protocol_send_byte(bytes[i], 1);
		checksum += bytes[i];
	}

	checksum = 0xff - checksum;

	_protocol_send_byte(checksum, 1);
}

uint8_t protocol_geterroror(){
	return error;
}

void protocol_receive_byte(uint8_t b){
	if (error > 0 && b == START) {
		// recover from any previous error condition
		error = 0;
		position = 0;
	} else if (error > 0) {
		return;
	}
	
	if (position > 0 && b == START) {
		// unexpected start of frame
		error = PROTOCOL_ERROR_UNEXPECTED_START_OF_FRAME;
		return;
	}
	if (position > 0 && b == ESCAPE) {
		// unescape next byte
		escape = 1;
		return;
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
			return;
		case 1: // length
			if (b == 0){
				error = PROTOCOL_ERROR_INVALID_LENGTH;
			}
			else {
				length = b;
				position++;
			}
			return;
		case 2:
			command = b;
			position++;
			return;
		default:
			if (position > MAX_SIZE) return;
			if (position == (length + 2)) {
				if (checksum == 0xff) {
					protocol_dispatch_message(command, message, length - 1);
				} else {
					error = PROTOCOL_ERROR_INVALID_CHECKSUM;
				}
				position = 0;
				checksum = 0;
			} else {
				message[position++ - 3] = b;
			}
	}
}
