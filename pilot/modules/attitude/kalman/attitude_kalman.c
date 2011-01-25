#include <math.h>

#include "../../../main.h"
#include "../../../lib/timer/timer.h"
#include "../../protocol/protocol.h"
#include "../../persist/persist.h"
#include "../../util/convert.h"

typedef struct kalman {
	// state
	double angle;
	double bias;
	double p00;
	double p01;
	double p10;
	double p11;
	// tuning
	double q_gyro;
	double q_angle;
	double r_angle;
} kalman_t;

typedef struct runge_kutta {
	// state
	double angle;
	double history[3];
	// tuning
	double scale;
	double bias;
} runge_kutta_t;
    
static kalman_t state_x;
static kalman_t state_y;
static runge_kutta_t state_z;

static uint64_t millis;

void attitude_read_tuning() {
	uint8_t data[36];
	
	uint8_t length = persist_read(PERSIST_SECTION_ATTITUDE, data, 36);
	if (length == 36) {
		state_x.q_angle = convert_bytes_to_double(data, 0);
		state_y.q_angle = convert_bytes_to_double(data, 4);
		state_z.scale = convert_bytes_to_double(data, 8);
		state_x.q_gyro = convert_bytes_to_double(data, 12);
		state_y.q_gyro = convert_bytes_to_double(data, 16);
		state_z.bias = convert_bytes_to_double(data, 20);
		state_x.r_angle = convert_bytes_to_double(data, 24);
		state_y.r_angle = convert_bytes_to_double(data, 28);
	} else {
		state_x.q_angle = 0.001;
		state_y.q_angle = 0.001;
		state_z.scale = 1.0; //(3.3 / 1024.0) / 0.002 * (M_PI / 2.0);
		state_x.q_gyro = 0.003;
		state_y.q_gyro = 0.003;
		state_z.bias = 0;
		state_x.r_angle = 0.3;
		state_y.r_angle = 0.3;
	}
}

void attitude_write_tuning() {
	uint8_t data[36];
	
	convert_double_to_bytes(state_x.q_angle, data, 0);
	convert_double_to_bytes(state_y.q_angle, data, 4);
	convert_double_to_bytes(state_z.scale, data, 8);
	convert_double_to_bytes(state_x.q_gyro, data, 12);
	convert_double_to_bytes(state_y.q_gyro, data, 16);
	convert_double_to_bytes(state_z.bias, data, 20);
	convert_double_to_bytes(state_x.r_angle, data, 24);
	convert_double_to_bytes(state_y.r_angle, data, 28);
	convert_double_to_bytes(0, data, 32);
	
	persist_write(PERSIST_SECTION_ATTITUDE, data, 36);
}

void attitude_init(vector_t gyro, vector_t accel) {
	timer_init();
	
	millis = timer_millis();

	attitude_read_tuning();
}

void _attitude (double gyro, double accel, kalman_t *state, double dt) {
	// http://tom.pycke.be/mav/71/kalman-filtering-of-imu-data
	
	state->angle += dt * (gyro - state->bias);
	state->p00 +=  - dt * (state->p10 + state->p01) + state->q_angle * dt;
	state->p01 +=  - dt * state->p11;
	state->p10 +=  - dt * state->p11;
	state->p11 +=  + state->q_gyro * dt;

	double y = accel - state->angle;
	double s = state->p00 + state->r_angle;
	double k0 = state->p00 / s;
	double k1 = state->p10 / s;

	state->angle +=  k0 * y;
	state->bias +=  k1 * y;
	state->p00 -= k0 * state->p00;
	state->p01 -= k0 * state->p01;
	state->p10 -= k1 * state->p00;
	state->p11 -= k1 * state->p01;
}

void _attitude_z(double gyro, runge_kutta_t *state, double dt) {
	// http://tom.pycke.be/mav/70/gyroscope-to-roll-pitch-and-yaw

	// state->angle = state->angle + gyro * state->scale * dt;

	// double corrected = gyro + state->bias; // should it be gyro * bias?
	double corrected = gyro * state->scale * dt;
	state->angle = state->angle + ((state->history[2] + state->history[1] * 2 + state->history[0] * 2 + corrected) / 6);
	state->history[2] = state->history[1];
	state->history[1] = state->history[0];
	state->history[0] = corrected;
	
	
}

vector_t attitude(vector_t gyro, vector_t accel) {
	uint64_t curr_millis = timer_millis();
	uint64_t dtm = curr_millis - millis;

	// if there is no elapsed time (ms), return the last known angle
	if (dtm > 0) {
		double dt = dtm * 0.001;
		millis = curr_millis;

		_attitude(gyro.x, accel.x, &state_x, dt);
		_attitude(gyro.y, accel.y, &state_y, dt);
		_attitude_z(gyro.z, &state_z, dt);
	}
	
	static vector_t angle;
	angle.x = state_x.angle;
	angle.y = state_y.angle;
	angle.z = state_z.angle * state_z.scale;
	return angle;
}

uint8_t attitude_get_id() {
	return 'K';
}

void attitude_reset() {
	state_x.angle = 0.0;
	state_y.angle = 0.0;
	state_z.angle = 0.0;
}

void attitude_send_tuning() {
	uint8_t length = 36;
	uint8_t buf[length];
	
	convert_double_to_bytes(state_x.q_angle, buf, 0);
	convert_double_to_bytes(state_y.q_angle, buf, 4);
	convert_double_to_bytes(state_z.scale, buf, 8);
	convert_double_to_bytes(state_x.q_gyro, buf, 12);
	convert_double_to_bytes(state_y.q_gyro, buf, 16);
	convert_double_to_bytes(state_z.bias, buf, 20);
	convert_double_to_bytes(state_x.r_angle, buf, 24);
	convert_double_to_bytes(state_y.r_angle, buf, 28);
	convert_double_to_bytes(0, buf, 32);

	protocol_send_message('k', buf, length);
}

void attitude_receive_tuning(uint8_t *buf) {
	state_x.q_angle = convert_bytes_to_double(buf, 0);
	state_y.q_angle = convert_bytes_to_double(buf, 4);
	state_z.scale = convert_bytes_to_double(buf, 8);
	state_x.q_gyro = convert_bytes_to_double(buf, 12);
	state_y.q_gyro = convert_bytes_to_double(buf, 16);
	state_z.bias = convert_bytes_to_double(buf, 20);
	state_x.r_angle = convert_bytes_to_double(buf, 24);
	state_y.r_angle = convert_bytes_to_double(buf, 28);
}

