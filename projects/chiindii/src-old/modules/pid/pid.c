#include "pid.h"
#include "../persist/persist.h"
#include "../util/convert.h"

typedef struct pid {
	// state
	double i;	// the integrated error
	double e;	// the last error
	// tuning
	double kp;
	double ki;
	double kd;
} pid_t;

static pid_t state_x;
static pid_t state_y;
// static pid_t state_z;
static vector_t last_mv; // used when dt == 0

void _pid_read_tuning() {
	uint8_t data[36];
	
	uint8_t length = persist_read(PERSIST_SECTION_PID, data, 36);
	if (length == 36) {
		state_x.kp = convert_bytes_to_double(data, 0);
		state_y.kp = convert_bytes_to_double(data, 4);
		// state_z.kp = convert_bytes_to_double(data, 8);
		state_x.ki = convert_bytes_to_double(data, 12);
		state_y.ki = convert_bytes_to_double(data, 16);
		// state_z.ki = convert_bytes_to_double(data, 20);
		state_x.kd = convert_bytes_to_double(data, 24);
		state_y.kd = convert_bytes_to_double(data, 28);
		// state_z.kd = convert_bytes_to_double(data, 32);
	} else {
		state_x.kp = 0.14;
		state_y.kp = 0.14;
		// state_z.kp = 0.14;
		state_x.ki = 0.0012;
		state_y.ki = 0.0012;
		// state_z.ki = 0.0012;
		state_x.kd = 0.03;
		state_y.kd = 0.03;
		// state_z.kd = 0.03;
	}
}

void _pid_write_tuning() {
	uint8_t data[36];
	
	convert_double_to_bytes(state_x.kp, data, 0);
	convert_double_to_bytes(state_y.kp, data, 4);
	// convert_double_to_bytes(state_z.kp, data, 8);
	convert_double_to_bytes(state_x.ki, data, 12);
	convert_double_to_bytes(state_y.ki, data, 16);
	// convert_double_to_bytes(state_z.ki, data, 20);
	convert_double_to_bytes(state_x.kd, data, 24);
	convert_double_to_bytes(state_y.kd, data, 28);
	// convert_double_to_bytes(state_z.kd, data, 32);
	
	persist_write(PERSIST_SECTION_PID, data, 36);
}

void pid_init() {
	_pid_read_tuning();
}

void pid_send_tuning() {
	uint8_t length = 36;
	uint8_t buf[36];
	
	convert_double_to_bytes(state_x.kp, buf, 0);
	convert_double_to_bytes(state_y.kp, buf, 4);
	// convert_double_to_bytes(state_z.kp, buf, 8);
	convert_double_to_bytes(state_x.ki, buf, 12);
	convert_double_to_bytes(state_y.ki, buf, 16);
	// convert_double_to_bytes(state_z.ki, buf, 20);
	convert_double_to_bytes(state_x.kd, buf, 24);
	convert_double_to_bytes(state_y.kd, buf, 28);
	// convert_double_to_bytes(state_z.kd, buf, 32);

	protocol_send_message('p', buf, length);
}

void pid_receive_tuning(uint8_t *buf) {
	state_x.kp = convert_bytes_to_double(buf, 0);
	state_y.kp = convert_bytes_to_double(buf, 4);
	// state_z.kp = convert_bytes_to_double(buf, 8);
	state_x.ki = convert_bytes_to_double(buf, 12);
	state_y.ki = convert_bytes_to_double(buf, 16);
	// state_z.ki = convert_bytes_to_double(buf, 20);
	state_x.kd = convert_bytes_to_double(buf, 24);
	state_y.kd = convert_bytes_to_double(buf, 28);
	// state_z.kd = convert_bytes_to_double(buf, 32);
	
	_pid_write_tuning();
}

double _pid_mv(double sp, double pv, pid_t *state, double dt){
	double error = sp - pv;
	state->i += (error * dt);
	double derivative = (error - state->e) / dt;
	
	double mv = (state->kp * error) + (state->ki * state->i) + (state->kd * derivative);

	state->e = error;

	return mv;
}

vector_t pid_mv(vector_t sp, vector_t pv, double dt) {
	vector_t mv;

	// for x and y, values are in radians
	mv.x = _pid_mv(sp.x, pv.x, &state_x, dt);
	mv.y = _pid_mv(sp.y, pv.y, &state_y, dt);

	last_mv.x = mv.x;
	last_mv.y = mv.y;

	// for z, values are in radians / second
	mv.z = sp.z;
	
	return mv;
}

void pid_reset() {
	state_x.i = 0;
	state_x.e = 0;
	state_y.i = 0;
	state_y.e = 0;
}

