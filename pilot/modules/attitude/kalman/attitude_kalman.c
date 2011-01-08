#include <math.h>

#include "../../../main.h"
#include "../../../lib/timer/timer.h"
#include "../../protocol/protocol.h"
#include "../../persist/persist.h"
#include "../../util/convert.h"

typedef struct kalman_state {
    double angle;
    double bias;
    double p00;
    double p01;
    double p10;
    double p11;
} kalman_state_t;

typedef struct kalman_tuning {
    double q_gyro;
    double q_angle;
    double r_angle;
} kalman_tuning_t;
    
static vector_t angle;
static kalman_state_t state_x;
static kalman_state_t state_y;
static kalman_state_t state_z;
static kalman_tuning_t tuning_x;
static kalman_tuning_t tuning_y;
static kalman_tuning_t tuning_z;

static uint64_t millis;

void attitude_read_tuning() {
	uint8_t data[36];
	
	uint8_t length = persist_read(PERSIST_SECTION_ATTITUDE, data, 36);
	if (length == 36) {
		tuning_x.q_angle = convert_bytes_to_double(data, 0);
		tuning_y.q_angle = convert_bytes_to_double(data, 4);
		tuning_z.q_angle = convert_bytes_to_double(data, 8);
		tuning_x.q_gyro = convert_bytes_to_double(data, 12);
		tuning_y.q_gyro = convert_bytes_to_double(data, 16);
		tuning_z.q_gyro = convert_bytes_to_double(data, 20);
		tuning_x.r_angle = convert_bytes_to_double(data, 24);
		tuning_y.r_angle = convert_bytes_to_double(data, 28);
		tuning_z.r_angle = convert_bytes_to_double(data, 32);
	} else {
		tuning_x.q_angle = 0.001;
		tuning_y.q_angle = 0.001;
		tuning_z.q_angle = 0.001;
		tuning_x.q_gyro = 0.003;
		tuning_y.q_gyro = 0.003;
		tuning_z.q_gyro = 0.003;
		tuning_x.r_angle = 0.3;
		tuning_y.r_angle = 0.3;
		tuning_z.r_angle = 0.3;
	}
}

void attitude_write_tuning() {
	uint8_t data[36];
	
	convert_double_to_bytes(tuning_x.q_angle, data, 0);
	convert_double_to_bytes(tuning_y.q_angle, data, 4);
	convert_double_to_bytes(tuning_z.q_angle, data, 8);
	convert_double_to_bytes(tuning_x.q_gyro, data, 12);
	convert_double_to_bytes(tuning_y.q_gyro, data, 16);
	convert_double_to_bytes(tuning_z.q_gyro, data, 20);
	convert_double_to_bytes(tuning_x.r_angle, data, 24);
	convert_double_to_bytes(tuning_y.r_angle, data, 28);
	convert_double_to_bytes(tuning_z.r_angle, data, 32);
	
	persist_write(PERSIST_SECTION_ATTITUDE, data, 36);
}

void attitude_init(vector_t gyro, vector_t accel) {
	timer_init();
	
    millis = timer_millis();

	attitude_read_tuning();
}

void _attitude (double gyro, double accel, kalman_state_t *state, kalman_tuning_t tuning, double dt) {
    state->angle += dt * (gyro - state->bias);
    state->p00 +=  - dt * (state->p10 + state->p01) + tuning.q_angle * dt;
    state->p01 +=  - dt * state->p11;
    state->p10 +=  - dt * state->p11;
    state->p11 +=  + tuning.q_gyro * dt;

    double y = accel - state->angle;
    double s = state->p00 + tuning.r_angle;
    double k0 = state->p00 / s;
    double k1 = state->p10 / s;

    state->angle +=  k0 * y;
    state->bias +=  k1 * y;
    state->p00 -= k0 * state->p00;
    state->p01 -= k0 * state->p01;
    state->p10 -= k1 * state->p00;
    state->p11 -= k1 * state->p01;
}

vector_t attitude(vector_t gyro, vector_t accel) {
    uint64_t curr_millis = timer_millis();
    uint64_t dtm = curr_millis - millis;

    //If there is no elapsed time (ms), return the last known angle.
    // If there is a dt==0 encountered, Kalman throws a fit!
    if (dtm == 0) return angle;

    double dt = dtm * 0.001;
    millis = curr_millis;

    _attitude(gyro.x, accel.x, &state_x, tuning_x, dt);
    _attitude(gyro.y, accel.y, &state_y, tuning_y, dt);
    _attitude(gyro.z, accel.z, &state_z, tuning_z, dt);
    angle.x = state_x.angle;
    angle.y = state_y.angle;
    angle.z = state_z.angle;
    return angle;
}

uint8_t attitude_get_id() {
    return 'K';
}

void attitude_reset() {
	angle.x = 0.0;
	angle.y = 0.0;
	angle.z = 0.0;
}

void attitude_send_tuning() {
	uint8_t length = 36;
	uint8_t buf[length];
	
	convert_double_to_bytes(tuning_x.q_angle, buf, 0);
	convert_double_to_bytes(tuning_y.q_angle, buf, 4);
	convert_double_to_bytes(tuning_z.q_angle, buf, 8);
	convert_double_to_bytes(tuning_x.q_gyro, buf, 12);
	convert_double_to_bytes(tuning_y.q_gyro, buf, 16);
	convert_double_to_bytes(tuning_z.q_gyro, buf, 20);
	convert_double_to_bytes(tuning_x.r_angle, buf, 24);
	convert_double_to_bytes(tuning_y.r_angle, buf, 28);
	convert_double_to_bytes(tuning_z.r_angle, buf, 32);

	protocol_send_message('k', buf, length);
}

void attitude_receive_tuning(uint8_t *buf) {
	tuning_x.q_angle = convert_bytes_to_double(buf, 0);
	tuning_y.q_angle = convert_bytes_to_double(buf, 4);
	tuning_z.q_angle = convert_bytes_to_double(buf, 8);
	tuning_x.q_gyro = convert_bytes_to_double(buf, 12);
	tuning_y.q_gyro = convert_bytes_to_double(buf, 16);
	tuning_z.q_gyro = convert_bytes_to_double(buf, 20);
	tuning_x.r_angle = convert_bytes_to_double(buf, 24);
	tuning_y.r_angle = convert_bytes_to_double(buf, 28);
	tuning_z.r_angle = convert_bytes_to_double(buf, 32);
}

