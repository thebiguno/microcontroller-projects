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

static uint8_t _last_flight_cmd; // only for Attitude and Motor messages
static uint8_t _last_flight_val[4];
static uint8_t _telemetry_enabled = 0;
static uint8_t _raw_enabled = 0;

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
	
	status_set(STATUS_MESSAGE_TX);
}

void _protocol_dispatch(uint8_t cmd, uint8_t length) {
	status_set(STATUS_MESSAGE_RX);
	switch(cmd) {
		case 'A':
			for (uint8_t i = 0; i < 4; i++) {
				_last_flight_val[i] = (_buf)[i];
			}
			_last_flight_cmd = cmd;
			break;
		case 'M':
			_last_flight_cmd = cmd;
			break;
		case 'C':
			gyro_calibrate();
			accel_calibrate();
			protocol_send_diag("calibrated");
			break;
		case 't':
			pid_send_tuning();
			attitude_send_tuning();
			protocol_send_diag("tuning sent");
			break;
		case 'E':
			_telemetry_enabled = _telemetry_enabled ? 0x00 : 0x01;
			break;
		case 'r':
			_raw_enabled = _raw_enabled ? 0x00 : 0x01;
			break;
		case 'p':
			pid_receive_tuning(_buf);
			protocol_send_diag("pid received");
			break;
		case 'c':
		case 'k':
			if (attitude_get_id() == 'K') {
				attitude_receive_tuning(_buf);
				protocol_send_diag("kalman received");				
			} else if (attitude_get_id() == 'C') {
				attitude_receive_tuning(_buf);
				protocol_send_diag("complementary received");
			}
			break;
		}
}

void protocol_poll()
{
    uint8_t b;
    while (comm_available() && comm_read(&b)) {
        if (_err > 0 && b == START) {
			// recover from error condition
			status_error(0x00);
			_err = 0;
			_pos = 0;
        } else if (_err > 0) {
            continue;
        }
        
        if (_pos > 0 && b == START) {
			// unexpected start of frame
			status_error(STATUS_ERR_PROTOCOL);
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
						_protocol_dispatch(_api, _len - 1);
					} else {
						status_error(STATUS_ERR_PROTOCOL);
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
		values[0] = convert_byte_to_percent(_last_flight_val[0]);
		values[1] = convert_byte_to_radian(_last_flight_val[1]);
		values[2] = convert_byte_to_radian(_last_flight_val[2]);
		values[3] = convert_byte_to_radian(_last_flight_val[3]);
		_last_flight_cmd = 0x00;
		return 'A';
	} else if (_last_flight_cmd == 'M') {
		values[0] = 0.0;
		values[1] = 0.0;
		values[2] = 0.0;
		values[3] = 0.0;
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

void protocol_send_raw(vector_t gyro, vector_t accel) {
	if (_raw_enabled) {
		uint8_t packet[24];
		
		convert_double_to_bytes(gyro.x, packet, 0);
		convert_double_to_bytes(gyro.y, packet, 4);
		convert_double_to_bytes(gyro.z, packet, 8);
		convert_double_to_bytes(accel.x, packet, 12);
		convert_double_to_bytes(accel.y, packet, 16);
		convert_double_to_bytes(accel.z, packet, 20);
		
		protocol_send_message('R', packet, 24);
	}
}

void protocol_send_diag(char* s) {
	uint8_t packet[255];
	uint8_t l = 0;
	while (*s != '\0') {
		packet[l] = *s;
		l++;
		s++;
	}
	protocol_send_message('D', packet, l);
}

void protocol_send_battery(double percent) {
	uint8_t packet[1];
	packet[0] = convert_percent_to_byte(percent);
	protocol_send_message('B', packet, 1);
}

