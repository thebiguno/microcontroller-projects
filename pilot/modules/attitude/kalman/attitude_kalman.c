#include <math.h>

#include "../../../main.h"
#include "../../../lib/timer/timer.h"

#define ATTITUDE_ALGORITHM_ID 0x01

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
static kalman_tuning_t tuning_x;
static kalman_tuning_t tuning_y;

static uint64_t millis;

void attitude_init(vector_t gyro, vector_t accel) {
    millis = timer_millis();

    // TODO read this from EEPROM
    tuning_x.q_angle = 0.001;
    tuning_y.q_angle = 0.001;
    tuning_x.q_gyro = 0.003;
    tuning_y.q_gyro = 0.003;
    tuning_x.r_angle = 0.03;
    tuning_y.r_angle = 0.03;
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
    if (dtm == 0) return angle;
    double dt = dtm * 0.001;
    millis = curr_millis;

    _attitude(gyro.x, accel.x, &state_x, tuning_x, dt);
    _attitude(gyro.y, accel.y, &state_y, tuning_y, dt);
    angle.x = state_x.angle;
    angle.y = state_y.angle;
    angle.z = gyro.z;
    return angle;
}

uint8_t attitude_get_id() {
	return ATTITUDE_ALGORITHM_ID;
}

void attitude_reset() {
	angle.x = 0.0;
	angle.y = 0.0;
}

void attitude_get_params(double params[]) {
    params[0] = tuning_x.q_angle;
    params[1] = tuning_y.q_angle;
    params[2] = tuning_x.q_gyro;
    params[3] = tuning_y.q_gyro;
    params[4] = tuning_x.r_angle;
    params[5] = tuning_y.r_angle;
}

void attitude_set_params(double params[]) {
    tuning_x.q_angle = params[0];
    tuning_y.q_angle = params[1];
    tuning_x.q_gyro = params[2];
    tuning_y.q_gyro = params[3];
    tuning_x.r_angle = params[4];
    tuning_y.r_angle = params[5];
}

