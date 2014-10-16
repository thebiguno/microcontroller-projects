#include "protocol.h"

#define START 0x7e
#define ESCAPE 0x7d

#define MAX_SIZE 40

static uint8_t _pos;	   // current position in the frame
static uint8_t _len;	   // frame length
static uint8_t _api;	   // frame api code (command)
static uint8_t _chk;	   // checksum
static uint8_t _esc;	   // escape byte seen, unescape next byte
static uint8_t _err;	   // error condition, ignore bytes until next frame start byte
static uint8_t _buf[MAX_SIZE];

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

uint8_t protocol_get_error(){
	return _err;
}

void protocol_receive_byte(uint8_t b){
	if (_err > 0 && b == START) {
		// recover from any previous error condition
		_err = 0;
		_pos = 0;
	} else if (_err > 0) {
		return;
	}
	
	if (_pos > 0 && b == START) {
		// unexpected start of frame
		_err = PROTOCOL_ERROR_UNEXPECTED_START_OF_FRAME;
		return;
	}
	if (_pos > 0 && b == ESCAPE) {
		// unescape next byte
		_esc = 1;
		return;
	}
	if (_esc) {
		// unescape current byte
		b = 0x20 ^ b;
		_esc = 0;
	}
	if (_pos > 1) { // start byte and length byte not included in checksum
		_chk += b;
	}
	
	switch(_pos) {
		case 0: // start frame
			_pos++;
			return;
		case 1: // length
			if (b == 0){
				_err = PROTOCOL_ERROR_INVALID_LENGTH;
			}
			else {
				_len = b;
				_pos++;
			}
			return;
		case 2:
			_api = b;
			_pos++;
			return;
		default:
			if (_pos > MAX_SIZE) return;
			if (_pos == (_len + 2)) {
				if (_chk == 0xff) {
					protocol_dispatch_message(_api, _buf, _len - 1);
				} else {
					_err = PROTOCOL_ERROR_INVALID_CHECKSUM;
				}
				_pos = 0;
				_chk = 0;
			} else {
				_buf[_pos++ - 3] = b;
			}
	}
}
