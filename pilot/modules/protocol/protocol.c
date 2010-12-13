#include "protocol.h"
#include "../comm/communication.h"

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
        comm_write(ESCAPE);
        comm_write(b ^ 0x20);
    } else {
        comm_write(b);
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

void _protocol_dispatch(uint8_t cmd, uint8_t length) {
	switch(cmd) {
		case 'A':
		case 'M':
			for (uint8_t i = 0; i < 4; i++) {
				_last_flight_val[i] = (_buf)[i];
			}
			_last_flight_cmd = cmd;
			break;
		case 'R':
			attitude_reset();
			break;
		case 'C':
			gyro_calibrate();
			accel_calibrate();
			break;
		case 't':
			pid_send_tuning();
			attitude_send_tuning();
			break;
		case 'E':
			_telemetry_enabled = 1;
			break;
		case 'e':
			_telemetry_enabled = 0;
			break;
		case 'W':
			//pid_persist();
			//attitude_persist();
			break;
		case 'p':
			pid_receive_tuning(_buf);
			break;
		case 'c':
			if (attitude_get_id() == 'C')
				attitude_receive_tuning(_buf);
			break;
		case 'k':
			if (attitude_get_id() == 'K')
				attitude_receive_tuning(_buf);
			break;
		case 'm':
			if (attitude_get_id() == 'M')
				attitude_receive_tuning(_buf);
			break;
		}
}

void protocol_poll()
{
    uint8_t b;
    while (comm_available() && comm_read(&b)) {
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
						_protocol_dispatch(_api, _len - 1);
						break; // i.e. maximum one message processed per main loop
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

uint8_t protocol_receive_flight_command(double values[])
{
	if (_last_flight_cmd == 'A') {
		values[0] = protocol_byte_to_percent(_last_flight_val[0]);
		values[1] = protocol_byte_to_radian(_last_flight_val[1]);
		values[2] = protocol_byte_to_radian(_last_flight_val[2]);
		values[3] = protocol_byte_to_radian(_last_flight_val[3]);
		_last_flight_cmd = 0x00;
		return 'A';
	} else if (_last_flight_cmd == 'M') {
		values[0] = protocol_byte_to_percent(_last_flight_val[0]);
		values[1] = protocol_byte_to_percent(_last_flight_val[1]);
		values[2] = protocol_byte_to_percent(_last_flight_val[2]);
		values[3] = protocol_byte_to_percent(_last_flight_val[3]);
		_last_flight_cmd = 0x00;
		return 'M';
	} else {
		return 0x00;
	}
}

void protocol_send_telemetry(vector_t vector, double motor[]) {
	if (_telemetry_enabled) {
		uint8_t packet[7];

		packet[0] = protocol_radian_to_byte(vector.x);
		packet[1] = protocol_radian_to_byte(vector.y);
		packet[2] = protocol_radian_to_byte(vector.z);
		for (uint8_t i = 0; i < 4; i++) {
			packet[i+3] = protocol_percent_to_byte(motor[i]);
		}
		protocol_send_message('T', packet, 7);
	}
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

