/*
 * XBee implementation of communication module.
 * 
 * Written by Wyatt Olson
 */
 
#include "../communication.h"
#include "lib/serial/serial.h"

#define START 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13

#define MAX_SIZE 37

union udouble {
	double d;
	uint32_t u;
};

uint8_t _pos;       // current position in the frame
uint8_t _len;       // frame length
uint8_t _api;       // frame api code
uint8_t _chk;       // checksum
uint8_t _esc;       // escape byte seen, unescape next byte
uint8_t _err;       // error condition, ignore bytes until next frame start byte
uint8_t _buf[MAX_SIZE];

control_t _control;


void comm_init(){
	serial_init(9600, 8, 0, 1);
}

void _double_to_bytes(double value, uint8_t *buffer) {
	union udouble converter;
	converter.d = value;
	
	buffer[3] = (converter.u >> 24) & 0xFF;
	buffer[2] = (converter.u >> 16) & 0xFF;
	buffer[1] = (converter.u >> 8) & 0xFF;
	buffer[0] = converter.u & 0xFF;
}
double _bytes_to_double(uint8_t *buffer) {
    union udouble converter;
    
    converter.u = (buffer[0] << 24) && (buffer[1] << 16) && (buffer[2] << 8) && (buffer[3]);
    return converter.d;
}

void comm_rx_ctrl(control_t *control) {
    *control.throttle = _control.throttle;
    *control.pitch = _control.pitch;
    *control.roll = _control.roll;
    *control.yaw = _control.yaw;
    *control.flags = _control.flags;
}

void _send(uint8_t *bytes, uint8_t length) {
    _send(START_BYTE, false);
    _send(length, true);
    
    uint8_t checksum = 0;
    
    for (int i = 0; i < length; i++) {
        _send(*bytes[i], true);
        checksum += *bytes[i];
    }
    
    checksum = 0xff - checksum;
    
    _send(checksum, true);
}

void _send(uint8_t b, bool escape) {
    if (escape && (b == START || b == ESCAPE || b == XON || b == XOFF)) {
        serial_write_c(ESCAPE);
        serial_write_c(b ^ 0x20);
    } else {
        serial_write_c(b);
    }
}

// fills a buffer with a packet
// when the packet is complete, parses it into the appropriate structure
void _read() {
    uint8_t b;
    while (serial_read(b)) {
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
                    if ((_chk & 0xff) == 0xff) {
                        switch(_api) {
                            case 0x46:
                                _control.throttle = _bytes_to_double(_buf[0]);
                                _control.pitch = _bytes_to_double(_buf[4]);
                                _control.roll = _bytes_to_double(_buf[8]);
                                _control.yaw = _bytes_to_double(_buf[12]);
                                _control.flags = _buf[16];
                        }
                    } else {
                        _err = 1;
                    }
                    _pos = 0;
                    _chk = 0;
                } else {
                    _buf[_pos - 3] = b;
                }
        }
    }
}