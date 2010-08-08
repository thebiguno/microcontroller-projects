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


void comm_init(){
	serial_init(9600, 8, 0, 1);
}

void _double_to_bytes(double value, uint8_t *buffer){
	union udouble converter;
	converter.d = value;
	
	buffer[3] = (converter.u >> 24) & 0xFF;
	buffer[2] = (converter.u >> 16) & 0xFF;
	buffer[1] = (converter.u >> 8) & 0xFF;
	buffer[0] = converter.u & 0xFF;
}

void _send_byte(uint8_t b, uint8_t escape) {
    if (escape && (b == START || b == ESCAPE || b == XON || b == XOFF)) {
        serial_write_c(ESCAPE);
        serial_write_c(b ^ 0x20);
    } else {
        serial_write_c(b);
    }
}

void _send_bytes(uint8_t *bytes, uint8_t length) {
    _send_byte(START, 0);
    _send_byte(length, 1);
    
    uint8_t checksum = 0;
    
    for (int i = 0; i < length; i++) {
        _send_byte(bytes[i], 1);
        checksum += bytes[i];
    }
    
    checksum = 0xff - checksum;
    
    _send_byte(checksum, 1);
}

void comm_tx_ctrl(control_t control, uint8_t flags){
	uint8_t packet[18];
    packet[0] = 0x46;
	_double_to_bytes(control.throttle, &packet[1]);
    _double_to_bytes(control.roll, &packet[5]);
    _double_to_bytes(control.pitch, &packet[9]);
    _double_to_bytes(control.yaw, &packet[13]);
    packet[17] = flags;
    _send_bytes(packet, 18);
}

