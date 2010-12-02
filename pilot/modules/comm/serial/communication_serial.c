/*
 * XBee implementation of communication module.
 * 
 * Written by Wyatt Olson
 */
 
#include "../communication.h"
#include "../../../lib/serial/serial.h"

#define START 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13

#define MAX_SIZE 36

union udouble {
	double d;
	uint8_t u[sizeof(double)];
};

uint8_t _pos;       // current position in the frame
uint8_t _len;       // frame length
uint8_t _api;       // frame api code
uint8_t _chk;       // checksum
uint8_t _esc;       // escape byte seen, unescape next byte
uint8_t _err;       // error condition, ignore bytes until next frame start byte
uint8_t _buf[MAX_SIZE];

uint8_t _flags = 0; // 0x01 = new command, 0x02 = new tuning

// cached values
double _command[4];
uint8_t _command_flags;
double _tuning[9];
uint8_t _tuning_type;

void comm_init(){
	serial_init(9600, 8, 0, 1);
}

void _double_to_bytes(double value, uint8_t *buffer) {
    union udouble converter;
    converter.d = value;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        buffer[i] = converter.u[i];
    }
}
double _bytes_to_double(uint8_t *buffer) {
    union udouble converter;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        converter.u[i] = buffer[0];
    }
    return converter.d;
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
    
    for (uint8_t i = 0; i < length; i++) {
        _send_byte(bytes[i], 1);
        checksum += bytes[i];
    }
    
    checksum = 0xff - checksum;
    
    _send_byte(checksum, 1);
}

// fills a buffer with a packet
// when the packet is complete, parses it into the appropriate structure
void _read() {
    uint8_t b;
    while (serial_available() && serial_read_c((char *) &b)) {
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
                            case 'C':
                                for (uint8_t i = 0; i < 4; i++) {
                                    _command[i] = _bytes_to_double(&_buf[i*4]);
                                }
                                _command_flags = _buf[16];
                                _command_flags = 0;
                                _flags |= _BV(1); // set the new message flag
                            case 'T':
                                _tuning_type = _buf[0];
                                for (uint8_t i = 0; i < 9; i++) {
                                    _tuning[0] = _bytes_to_double(&_buf[(i*4)+1]);
                                }
                                _flags |= _BV(2); // set the new message flag
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

uint8_t comm_rx_command(double command[], uint8_t *flags) {
    _read();
    if (_flags & _BV(1)) {
        // only copy values if the data has changed
        for (int i = 0; i < 4; i++) {
            command[i] = _command[i];
        }
        *flags = _command_flags;
        _flags &= ~_BV(1); // clear the new message flag
        return 0x01;
    } else {
        return 0x00;
    }
}

uint8_t comm_rx_tuning(uint8_t *type, double tuning[]) {
    _read();
    if (_flags & _BV(2)) {
        // only copy values if the data has changed
        *type = _tuning_type;
        for (int i = 0; i < 9; i++) {
            tuning[i] = _tuning[i];
        }
        _flags &= ~_BV(2); // clear the new message flag
        return 0x01;
    } else {
        return 0x00;
    }
}

void comm_tx_telemetry(vector_t vector, double motor[], uint8_t flags) {
	uint8_t packet[30];
    packet[0] = 'E';
	_double_to_bytes(vector.x, &packet[1]);
    _double_to_bytes(vector.y, &packet[5]);
    _double_to_bytes(vector.z, &packet[9]);
    for (uint8_t i = 0; i < 4; i++) {
        _double_to_bytes(motor[0], &packet[(i*4)+13]);
    }
    packet[29] = flags;
    _send_bytes(packet, 30);
}

void comm_tx_tuning(uint8_t type, double payload[]) {
	uint8_t packet[38];
    packet[0] = 'T';
    packet[1] = type;
    for (uint8_t i = 0; i < 9; i++) {
        _double_to_bytes(payload[0], &packet[(i*4)+2]);
    }
    _send_bytes(packet, 38);
}
