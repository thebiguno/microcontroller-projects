#include "protocol.h"
#include "../util/convert.h"
#include "../comm/communication.h"

#define START 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13

#define MAX_SIZE 40

static uint8_t _pos;       // current position in the frame
static uint8_t _len;       // frame length
static uint8_t _api;       // frame api code
static uint8_t _chk;       // checksum
static uint8_t _esc;       // escape byte seen, unescape next byte
static uint8_t _err;       // error condition, ignore bytes until next frame start byte
static uint8_t _buf[MAX_SIZE];

static void _protocol_send_byte(uint8_t b, uint8_t escape)
{
    if (escape && (b == START || b == ESCAPE || b == XON || b == XOFF)) {
        comm_pilot_write(ESCAPE);
        comm_pilot_write(b ^ 0x20);
    } else {
        comm_pilot_write(b);
    }
}

static void protocol_send_message(uint8_t cmd, uint8_t *bytes, uint8_t length)
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

void protocol_send_control(control_t control){
	uint8_t packet[18];
	convert_double_to_bytes(control.throttle, packet, 0);
	convert_double_to_bytes(control.pitch, packet, 4);
	convert_double_to_bytes(control.roll, packet, 8);
	convert_double_to_bytes(control.yaw, packet, 12);
	protocol_send_message('C', packet, 16);
}

void protocol_send_reset_attitude() {
	uint8_t packet[] = {0}; // zero sized array not allowed in C
	protocol_send_message('R', packet, 0);
}

/*
 * copy all bytes from the pilot to the pc without interpretting them
 */
void protocol_poll_pilot(){
	uint8_t b;
	while (comm_pilot_available() && comm_pilot_read(&b)) {
		comm_pc_write(b);
	}
}

/*
 * copy all messages from the pc to the pilot
 * messages are decoded and buffered so they can be retransmitted without interfering with messages from the controller
 */
void protocol_poll_pc(){
    uint8_t b;
    while (comm_pc_available() && comm_pc_read(&b)) {
        if (_err > 0 && b == START) {
			// recover from error condition
			_err = 0;
			_pos = 0;
        } else if (_err > 0) {
            continue;
        }
        
        if (_pos > 0 && b == START) {
			// unexpected start of frame
			_err = 1;
			continue;
        }
        if (_pos > 0 && b == ESCAPE) {
			// unescape next byte
			_esc = 1;
			continue;
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
                continue;
            case 1: // length
                _len = b;
                _pos++;
                continue;
            case 2:
                _api = b;
                _pos++;
                continue;
            default:
                if (_pos > MAX_SIZE) continue; // this probably can't happen since the xbee packet size is larger than any of our messages
                if (_pos == (_len + 2)) {
                    if (_chk == 0xff) {
						protocol_send_message(_api, _buf, _len - 1);
					} else {
						_err = 1;
                    }
                    _pos = 0;
                    _chk = 0;
                } else {
                    _buf[_pos++ - 3] = b;
                }
        }
    }
}

