#include <math.h>

#include "../../../main.h"
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

static kalman_t state_x;
static kalman_t state_y;

void attitude_read_tuning() {
	uint8_t data[24];
	
	uint8_t length = persist_read(PERSIST_SECTION_ATTITUDE, data, 24);
	if (length == 24) {
		state_x.q_angle = convert_bytes_to_double(data, 0);
		state_x.q_gyro = convert_bytes_to_double(data, 4);
		state_x.r_angle = convert_bytes_to_double(data, 8);

		state_y.q_angle = convert_bytes_to_double(data, 12);
		state_y.q_gyro = convert_bytes_to_double(data, 16);
		state_y.r_angle = convert_bytes_to_double(data, 20);
	} else {
		state_x.q_angle = 0.001;
		state_x.q_gyro = 0.003;
		state_x.r_angle = 0.3;

		state_y.q_angle = 0.001;
		state_y.q_gyro = 0.003;
		state_y.r_angle = 0.3;
	}
}

void attitude_write_tuning() {
	uint8_t data[24];
	
	convert_double_to_bytes(state_x.q_angle, data, 0);
	convert_double_to_bytes(state_y.q_angle, data, 4);
	convert_double_to_bytes(state_x.q_gyro, data, 8);
	
	convert_double_to_bytes(state_y.q_gyro, data, 12);
	convert_double_to_bytes(state_x.r_angle, data, 16);
	convert_double_to_bytes(state_y.r_angle, data, 20);
	
	persist_write(PERSIST_SECTION_ATTITUDE, data, 24);
}

void attitude_init(vector_t gyro, vector_t accel) {
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

vector_t attitude(vector_t gyro, vector_t accel, uint16_t dt) {
	// if there is no elapsed time (ms), return the last known angle
	if (dt > 0) {
		double dt_s = dt * 0.001;
		_attitude(gyro.x, accel.x, &state_x, dt_s);
		_attitude(gyro.y, accel.y, &state_y, dt_s);
	}
	
	static vector_t angle;
	angle.x = state_x.angle;
	angle.y = state_y.angle;
	return angle;
}

uint8_t attitude_get_id() {
	return 'K';
}

void attitude_reset() {
	state_x.angle = 0.0;
	state_y.angle = 0.0;
}

void attitude_send_tuning() {
	uint8_t length = 24;
	uint8_t buf[length];
	
	convert_double_to_bytes(state_x.q_angle, buf, 0);
	convert_double_to_bytes(state_x.q_gyro, buf, 4);
	convert_double_to_bytes(state_x.r_angle, buf, 8);

	convert_double_to_bytes(state_y.q_angle, buf, 12);
	convert_double_to_bytes(state_y.q_gyro, buf, 16);
	convert_double_to_bytes(state_y.r_angle, buf, 20);

	protocol_send_message('k', buf, length);
}

void attitude_receive_tuning(uint8_t *buf) {
	state_x.q_angle = convert_bytes_to_double(buf, 0);
	state_x.q_gyro = convert_bytes_to_double(buf, 4);
	state_x.r_angle = convert_bytes_to_double(buf, 8);

	state_y.q_angle = convert_bytes_to_double(buf, 12);
	state_y.q_gyro = convert_bytes_to_double(buf, 16);
	state_y.r_angle = convert_bytes_to_double(buf, 20);
}

