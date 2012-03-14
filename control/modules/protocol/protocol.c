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
static double battery = -1;	// < 0 means 'unknown'; 0..1 inclusive is valid range.
static double battery_misses = 0;	//Counter of how many times battery is read w/o being updated.  If this exceeds 3, we return -1.

#define MAILBOX_PID		_BV(0)
#define MAILBOX_MOTOR	_BV(1)
#define MAILBOX_COMP	_BV(2)
#define MAILBOX_KALMAN	_BV(3)
static uint8_t mailbox_flag = 0;	//Check bits for what message types are available

static vector_t _vector;
static vector_t pid_p, pid_i, pid_d;					//Mailbox for received PID tuning values
static double motor_tune[4] = {1.0, 1.0, 1.0, 1.0};		//Mailbox for received motor tuning values
static vector_t kalman_qa, kalman_qg, kalman_ra;		//Mailbox for received kalman tuning values
static vector_t comp_k;									//Mailbox for received complementary tuning values

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
	comm_state |= PROTOCOL_COMM_TX; //Controller is sending to pilot

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

void protocol_send_calibrate(){
	protocol_send_message_to_pilot('C', dummy, 0);
}

void protocol_send_toggle_telemetry(){
	protocol_send_message_to_pilot('E', dummy, 0);
}

void protocol_request_tuning(){
	protocol_send_message_to_pilot('t', dummy, 0);
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
			battery_misses = 0;
			break;
		case 'T':
			_vector.x = convert_byte_to_radian(pilot.buf[0]);
			_vector.y = convert_byte_to_radian(pilot.buf[1]);
			_motors[0] = convert_byte_to_percent(pilot.buf[3]);
			_motors[1] = convert_byte_to_percent(pilot.buf[4]);
			_motors[2] = convert_byte_to_percent(pilot.buf[5]);
			_motors[3] = convert_byte_to_percent(pilot.buf[6]);
			break;
		case 'p':
			pid_p.x = convert_bytes_to_double(pilot.buf, 0);
			pid_p.y = convert_bytes_to_double(pilot.buf, 4);
			//z is ignored
			pid_i.x = convert_bytes_to_double(pilot.buf, 12);
			pid_i.y = convert_bytes_to_double(pilot.buf, 16);
			//z is ignored
			pid_d.x = convert_bytes_to_double(pilot.buf, 24);
			pid_d.y = convert_bytes_to_double(pilot.buf, 28);
			mailbox_flag |= MAILBOX_PID;
			break;
		case 'k':
			kalman_qa.x = convert_bytes_to_double(pilot.buf, 0);
			kalman_qg.x = convert_bytes_to_double(pilot.buf, 4);
			kalman_ra.x = convert_bytes_to_double(pilot.buf, 8);
			kalman_qa.y = convert_bytes_to_double(pilot.buf, 12);
			kalman_qg.y = convert_bytes_to_double(pilot.buf, 16);
			kalman_ra.y = convert_bytes_to_double(pilot.buf, 20);
			mailbox_flag |= MAILBOX_KALMAN;
			break;
		case 'm':
			motor_tune[0] = convert_bytes_to_double(pilot.buf, 0);
			motor_tune[1] = convert_bytes_to_double(pilot.buf, 4);
			motor_tune[2] = convert_bytes_to_double(pilot.buf, 8);
			motor_tune[3] = convert_bytes_to_double(pilot.buf, 12);
			mailbox_flag |= MAILBOX_MOTOR;
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
					comm_state |= PROTOCOL_COMM_RX;	//Controller is recieving message
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

void protocol_get_pid_tuning(vector_t *p, vector_t *i, vector_t *d){
	if (mailbox_flag & MAILBOX_PID){
		p->x = pid_p.x;
		i->x = pid_i.x;
		d->x = pid_d.x;
		p->y = pid_p.y;
		i->y = pid_i.y;
		d->y = pid_d.y;
	}
	
	//Reset flag
	mailbox_flag &= ~MAILBOX_PID;
}

void protocol_get_motor_tuning(double *motor){
	if (mailbox_flag & MAILBOX_MOTOR){
		for (uint8_t i = 0; i < 4; i++){
			motor[i] = motor_tune[i];
		}
	}
	
	//Reset flag
	mailbox_flag &= ~MAILBOX_MOTOR;
}

void protocol_get_comp_tuning(vector_t *k){
	if (mailbox_flag & MAILBOX_COMP){
		k->x = comp_k.x;
		k->y = comp_k.y;
	}
	
	//Reset flag
	mailbox_flag &= ~MAILBOX_COMP;
}

void protocol_get_kalman_tuning(vector_t *qa, vector_t *qg, vector_t *ra){
	if (mailbox_flag & MAILBOX_KALMAN){
		qa->x = kalman_qa.x;
		qg->x = kalman_qg.x;
		ra->x = kalman_ra.x;
		qa->y = kalman_qa.y;
		qg->y = kalman_qg.y;
		ra->y = kalman_ra.y;
	}
	
	//Reset flag
	mailbox_flag &= ~MAILBOX_KALMAN;
}



void protocol_send_pid_tuning(vector_t p, vector_t i, vector_t d){
	uint8_t data[36];
	convert_double_to_bytes(p.x, data, 0);
	convert_double_to_bytes(p.y, data, 4);
	//z is ignored
	convert_double_to_bytes(i.x, data, 12);
	convert_double_to_bytes(i.y, data, 16);
	//z is ignored					
	convert_double_to_bytes(d.x, data, 24);
	convert_double_to_bytes(d.y, data, 28);
	//z is ignored					
	protocol_send_message_to_pilot('p', data, 36);
}

void protocol_send_motor_tuning(double motors[]){
	uint8_t data[16];
	convert_double_to_bytes(motors[0], data, 0);
	convert_double_to_bytes(motors[1], data, 4);
	convert_double_to_bytes(motors[2], data, 8);
	convert_double_to_bytes(motors[3], data, 12);
	protocol_send_message_to_pilot('m', data, 16);
}

void protocol_send_comp_tuning(vector_t k){
	uint8_t data[8];
	convert_double_to_bytes(k.x, data, 0);
	convert_double_to_bytes(k.y, data, 4);
	protocol_send_message_to_pilot('c', data, 8);	
}

void protocol_send_kalman_tuning(vector_t qa, vector_t qg, vector_t ra){
	uint8_t data[24];
	convert_double_to_bytes(qa.x, data, 0);
	convert_double_to_bytes(qg.x, data, 4);
	convert_double_to_bytes(ra.x, data, 8);
	convert_double_to_bytes(qa.y, data, 12);
	convert_double_to_bytes(qg.y, data, 16);
	convert_double_to_bytes(ra.y, data, 20);
	protocol_send_message_to_pilot('k', data, 24);	
}


void protocol_clear_comm_state() {
	comm_state = 0;
}

uint8_t protocol_comm_state(uint8_t bit) {
	return comm_state & bit;
}

double protocol_get_battery() {
	double temp = battery;
	battery_misses++;
	if (battery_misses >= 3) battery = -1.0;	//Reset if we have not gotten a reading for a while
	return temp;
}

void protocol_get_motors(double motors[]) {
	for (int i = 0; i < 4; i++) {
		motors[i] = _motors[i];
	}
}

vector_t protocol_get_vector() {
	vector_t result;
	result.x = _vector.x;
	result.y = _vector.y;

	//Reset	
	_vector.x = 10000;	//Anything over 1000 is read as 'invalid'.
	_vector.y = 10000;	//Anything over 1000 is read as 'invalid'.
	
	return result;
}