#include "pid.h"
#include "../persist/persist.h"
#include "../util/convert.h"

// See http://www.eetimes.com/ContentEETimes/Documents/Embedded.com/2000/f-wescot.pdf for code which
// this has been based off of.

//Integrator state bounds.  
// TODO this needs to be adjusted; perhaps this should be a tuning value as well?  Or perhaps
// use them in conjunction with the tuning values?
#define I_MAX 2.0
#define I_MIN -2.0

typedef struct pid {
	// state
	double d;			// Last position input
	double i;			// Integrator state

	// tuning gain values
	double kp;
	double ki;
	double kd;
} pid_t;

static pid_t state_x;
static pid_t state_y;
// static pid_t state_z;
static vector_t last_mv; // used when dt == 0

void pid_read_tuning() {
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

void pid_write_tuning() {
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
	pid_read_tuning();
}

void pid_send_tuning() {
	uint8_t length = 36;
	uint8_t buf[length];
	
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
}

double _pid_mv(double sp, double pv, pid_t *state, double dt){
	double error = sp - pv;
	double pTerm = state->kp * error;
	
	//Accumulate the integral state, within given bounds
	state->i += error;
	if (state->i > I_MAX) state->i = I_MAX;
	else if (state->i < I_MIN) state->i = I_MIN;
	
	double iTerm = state->ki * state->i;

	//PID for non-PHDs does not divide by dt (in fact does not use dt at all); I assume this
	// is because the paper states that you must have internal consistency in the reading
	// period.  TODO check chiindii reading consistency to make sure that we are reading at
	// a consistent rate, and if not, then take dt into account as well.
	double dTerm = state->kd * (state->d - pv);
	state->d = pv;
	
	return pTerm + iTerm + dTerm;
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
	state_x.d = 0;
	state_y.i = 0;
	state_y.d = 0;
}

