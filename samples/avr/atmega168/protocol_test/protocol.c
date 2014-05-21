#include "protocol.h"
#include "lib/serial/serial.h"

#define START 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13

#define MAX_SIZE 40

uint8_t _pos;       // current position in the frame
uint8_t _len;       // frame length
uint8_t _api;       // frame api code
uint8_t _chk;       // checksum
uint8_t _esc;       // escape byte seen, unescape next byte
uint8_t _err;       // error condition, ignore bytes until next frame start byte
uint8_t _buf[MAX_SIZE];

uint8_t _last_flight_cmd; // only for Attitude and Motor messages
uint8_t _last_flight_val[4];
uint8_t _telemetry_enabled = 0;

void _protocol_send_byte(uint8_t b, uint8_t escape)
{
    if (escape && (b == START || b == ESCAPE || b == XON || b == XOFF)) {
        serial_write_c(ESCAPE);
        serial_write_c(b ^ 0x20);
    } else {
        serial_write_c(b);
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

double protocol_byte_to_radian(uint8_t x) {
	double r = 0.024639942381096 * x;
	return (r > M_PI) ? (2.0 * M_PI) - r : r; // convert quadrants III & IV into negative values
}
uint8_t protocol_radian_to_byte(double x) {
	double r = (x < 0) ? (2.0 * M_PI) + x : x; // convert quadrants III & IV into positive values
	return (uint8_t) (40.584510488433314 * r);
}
double protocol_byte_to_percent(uint8_t x) {
	return 0.392156862745098 * x;
}
uint8_t protocol_percent_to_byte(double x) {
	return (uint8_t) (2.55 * x);
}
void protocol_double_to_bytes(double value, uint8_t *buffer, uint8_t offset) {
    union udouble converter;
    converter.d = value;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        buffer[i + offset] = converter.u[i];
    }
}
double protocol_bytes_to_double(uint8_t *buffer, uint8_t offset) {
    union udouble converter;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        converter.u[i] = buffer[i + offset];
    }
    return converter.d;
}

