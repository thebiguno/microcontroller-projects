#include "protocol.h"
#include "../util/convert.h"
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

uint8_t shift_state = 0x00;

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
	
	shift_state ^= _BV(2); // toggle every message sent
	shift_out(shift_state);
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
			_telemetry_enabled = _telemetry_enabled ? 0x00 : 0x01;
			shift_state ^= _BV(5);
			break;
		case 'W':
			pid_write_tuning();
			attitude_write_tuning();
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
	shift_state ^= _BV(0); // toggle every call to poll (heartbeat)
	shift_out(shift_state);
	
    uint8_t b;
    while (comm_available() && comm_read(&b)) {
        if (_err > 0 && b == START) {
			// recover from error condition
			shift_state &= ~_BV(7);
			_err = 0;
			_pos = 0;
        } else if (_err > 0) {
            continue;
        }
        
        if (_pos > 0 && b == START) {
			// unexpected start of frame
			shift_state |= _BV(7);
			_err = 1;
			continue;
        }
        if (_pos > 0 && b == ESCAPE) {
			// unescape next byte
			shift_state |= _BV(6);
			_esc = 1;
			continue;
        }
        if (_esc) {
			// unescape current byte
			b = 0x20 ^ b;
			shift_state &= ~_BV(6);
			_esc = 0;
        }
        if (_pos > 1) { // start byte and length byte not included in checksum
            _pos += b;
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
						shift_state ^= _BV(1); // toggle for every message dispatched
						_protocol_dispatch(_api, _len - 1);
						break; // i.e. maximum one message processed per main loop
					} else {
						shift_state |= _BV(7);
						_err = 1;
                    }
                    _pos = 0;
                    _chk = 0;
                } else {
                    _buf[_pos++ - 3] = b;
                }
        }
		shift_out(shift_state);
    }
}

uint8_t protocol_receive_flight_command(double values[])
{
	if (_last_flight_cmd == 'A') {
		values[0] = convert_byte_to_percent(_last_flight_val[0]);
		values[1] = convert_byte_to_radian(_last_flight_val[1]);
		values[2] = convert_byte_to_radian(_last_flight_val[2]);
		values[3] = convert_byte_to_radian(_last_flight_val[3]);
		_last_flight_cmd = 0x00;
		return 'A';
	} else if (_last_flight_cmd == 'M') {
		values[0] = convert_byte_to_percent(_last_flight_val[0]);
		values[1] = convert_byte_to_percent(_last_flight_val[1]);
		values[2] = convert_byte_to_percent(_last_flight_val[2]);
		values[3] = convert_byte_to_percent(_last_flight_val[3]);
		_last_flight_cmd = 0x00;
		return 'M';
	} else {
		return 0x00;
	}
}

void protocol_send_telemetry(vector_t vector, double motor[]) {
	if (_telemetry_enabled) {
		uint8_t packet[7];

		packet[0] = convert_radian_to_byte(vector.x);
		packet[1] = convert_radian_to_byte(vector.y);
		packet[2] = convert_radian_to_byte(vector.z);
		for (uint8_t i = 0; i < 4; i++) {
			packet[i+3] = convert_percent_to_byte(motor[i]);
		}
		protocol_send_message('T', packet, 7);
	}
}

