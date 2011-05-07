#include "protocol.h"
#include "../util/convert.h"
#include "../comm/communication.h"

#define START 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13

#define MAX_SIZE 40

typedef struct state {
	uint8_t pos;			// current position in the frame
	uint8_t len;			// frame length
	uint8_t api;			// frame api code
	uint8_t chk;			// checksum
	uint8_t esc;			// escape byte seen, unescape next byte
	uint8_t err;			// error condition, ignore bytes until next frame start byte
	uint8_t buf[MAX_SIZE];
} state_t;

// Set bits as messages sent / recieved.  Cleared by main 
// calling protocol_clear_activity_state().  Bits interpreted
// as PROTOCOL_COMM_{RX|TX}_{PILOT|PC} in protocol.h
static uint8_t comm_state = 0; 
static double battery = 0;
static double _vector[] = {0,0};
static double _motors[] = {0,0,0,0};

static state_t pilot;		// messages coming in from the pilot
static state_t pc;			// messages coming in from the pc

static uint8_t dummy[] = {0}; // zero sized array not allowed in C

static void _protocol_send_byte_to_pc(uint8_t b, uint8_t escape){
	if (escape && (b == START || b == ESCAPE || b == XON || b == XOFF)) {
		comm_pc_write(ESCAPE);
		comm_pc_write(b ^ 0x20);
	} else {
		comm_pc_write(b);
	}
}

static void protocol_send_message_to_pc(uint8_t cmd, uint8_t *bytes, uint8_t length){
	comm_state |= PROTOCOL_COMM_TX_PC;

	_protocol_send_byte_to_pc(START, 0);
	_protocol_send_byte_to_pc(length + 1, 1);
	_protocol_send_byte_to_pc(cmd, 0);
	
	uint8_t checksum = cmd;

	for (uint8_t i = 0; i < length; i++) {
		_protocol_send_byte_to_pc(bytes[i], 1);
		checksum += bytes[i];
	}

	checksum = 0xff - checksum;

	_protocol_send_byte_to_pc(checksum, 1);
}

static void _protocol_send_byte_to_pilot(uint8_t b, uint8_t escape){
    if (escape && (b == START || b == ESCAPE || b == XON || b == XOFF)) {
        comm_pilot_write(ESCAPE);
        comm_pilot_write(b ^ 0x20);
    } else {
        comm_pilot_write(b);
    }
}

static void protocol_send_message_to_pilot(uint8_t cmd, uint8_t *bytes, uint8_t length){
	comm_state |= PROTOCOL_COMM_TX_PILOT;

	_protocol_send_byte_to_pilot(START, 0);
	_protocol_send_byte_to_pilot(length + 1, 1);
	_protocol_send_byte_to_pilot(cmd, 0);
	
	uint8_t checksum = cmd;

	for (uint8_t i = 0; i < length; i++) {
		_protocol_send_byte_to_pilot(bytes[i], 1);
		checksum += bytes[i];
	}

	checksum = 0xff - checksum;

	_protocol_send_byte_to_pilot(checksum, 1);
}

void protocol_send_control(control_t control){
	uint8_t packet[4];
	packet[0] = convert_percent_to_byte(control.throttle);
	packet[1] = convert_radian_to_byte(control.pitch);
	packet[2] = convert_radian_to_byte(control.roll);
	packet[3] = convert_radian_to_byte(control.yaw);
	protocol_send_message_to_pilot('A', packet, 4);
}
void protocol_send_kill() {
	uint8_t packet[4];
	packet[0] = convert_percent_to_byte(0);
	packet[1] = convert_percent_to_byte(0);
	packet[2] = convert_percent_to_byte(0);
	packet[3] = convert_percent_to_byte(0);
	protocol_send_message_to_pilot('M', packet, 4);
}

void protocol_send_reset_attitude(){
	protocol_send_message_to_pilot('R', dummy, 0);
}

void protocol_send_calibrate(){
	protocol_send_message_to_pilot('C', dummy, 0);
}

void protocol_send_toggle_telemetry(){
	protocol_send_message_to_pilot('E', dummy, 0);
}

void protocol_send_diag(char* s) {
	uint8_t packet[255];
	uint8_t l = 0;
	while (*s != '\0') {
		packet[l] = *s;
		l++;
		s++;
	}
	protocol_send_message_to_pc('D', packet, l);
}

void _protocol_dispatch(uint8_t cmd, uint8_t length) {
	switch(cmd) {
		case 'B':
			battery = convert_byte_to_percent(pilot.buf[0]);
			break;
		case 'T':
			_vector[0] = convert_byte_to_radian(pilot.buf[0]);
			_vector[1] = convert_byte_to_radian(pilot.buf[1]);
			_motors[0] = convert_byte_to_percent(pilot.buf[3]);
			_motors[1] = convert_byte_to_percent(pilot.buf[4]);
			_motors[2] = convert_byte_to_percent(pilot.buf[5]);
			_motors[3] = convert_byte_to_percent(pilot.buf[6]);
			break;
	}
}

/*
 * intercept all messages from the pilot and dispatch them to the status module
 * copy all messages from source to destintation
 * messages are decoded and buffered so they can be retransmitted without interfering with messages from the controller
 * dest of 0x00 is pilot
 * dest of 0x01 is pc
 */
static void _protocol_poll(uint8_t b, state_t *source, uint8_t dest){
	if (source->err > 0 && b == START) {
		// recover from error condition
		source->err = 0;
		source->pos = 0;
	} else if (source->err > 0) {
		return;
	}

	if (source->pos > 0 && b == START) {
		// unexpected start of frame
		source->err = 1;
		return;
	}
	if (source->pos > 0 && b == ESCAPE) {
		// unescape next byte
		source->esc = 1;
		return;
	}
	if (source->esc) {
		// unescape current byte
		b = 0x20 ^ b;
		source->esc = 0;
	}
	if (source->pos > 1) { // start byte and length byte not included in checksum
		source->chk += b;
	}

	switch(source->pos) {
	case 0: // start frame
		source->pos++;
		return;
	case 1: // length
		source->len = b;
		source->pos++;
		return;
	case 2:
		source->api = b;
		source->pos++;
		return;
	default:
		if (source->pos > MAX_SIZE) return; // this probably can't happen since the xbee packet size is larger than any of our messages
		if (source->pos == (source->len + 2)) {
			if (source->chk == 0xff) {
				if (dest == 0x00) {
					protocol_send_message_to_pilot(source->api, source->buf, source->len - 1);
				} else if (dest == 0x01) {
					_protocol_dispatch(source->api, source->len - 1);
					protocol_send_message_to_pc(source->api, source->buf, source->len - 1);
				}
			} else {
				source->err = 1;
			}
			source->pos = 0;
			source->chk = 0;
		} else {
			source->buf[source->pos++ - 3] = b;
		}
	}
}

static void protocol_poll_pilot(){
	uint8_t b;
	while (comm_pilot_available() && comm_pilot_read(&b)) {
		_protocol_poll(b, &pilot, 0x01);
	}
}

static void protocol_poll_pc() {
	uint8_t b;
	while (comm_pc_available() && comm_pc_read(&b)) {
		_protocol_poll(b, &pc, 0x00);
	}
}

void protocol_poll() {
	protocol_poll_pilot();
	protocol_poll_pc();
}

void protocol_clear_comm_state() {
	comm_state = 0;
}

uint8_t protocol_comm_state(uint8_t bit) {
	return comm_state & bit;
}

double protocol_get_battery() {
	return battery;
}

void protocol_get_motors(double motors[]) {
	for (int i = 0; i < 4; i++) {
		motors[i] = _motors[i];
	}
}

void protocol_get_vector(double vector[]) {
	for (int i = 0; i < 2; i++) {
		vector[i] = _vector[i];
	}
}