#include <math.h>

#include "../../../main.h"
#include "../../../lib/timer/timer.h"

#define ATTITUDE_ALGORITHM_ID 0x01

static vector_t angle;
static vector_t bias;
static vector_t p00;
static vector_t p01;
static vector_t p10;
static vector_t p11;

static vector_t q_gyro;
static vector_t q_angle;
static vector_t r_angle;
static uint64_t millis;

void attitude_init(vector_t gyro, vector_t accel) {
    millis = timer_millis();

    // TODO read this from EEPROM
    q_angle.x = 0.001;
    q_angle.y = 0.001;
    q_gyro.x = 0.003;
    q_gyro.y = 0.003;
    r_angle.x = 0.03;
    r_angle.y = 0.03;
}

void _attitude (double gyro, double accel, double *angle, double *bias, double *p00, double *p01, double *p10, double *p11, double q_gyro, double q_angle, double r_angle, double dt) {
    *angle += dt * (gyro - *bias);
    *p00 +=  - dt * (*p10 + *p01) + q_angle * dt;
    *p01 +=  - dt * *p11;
    *p10 +=  - dt * *p11;
    *p11 +=  + q_gyro * dt;

    double y = accel - *angle;
    double s = *p00 + r_angle;
    double k0 = *p00 / s;
    double k1 = *p10 / s;

    *angle +=  k0 * y;
    *bias +=  k1 * y;
    *p00 -= k0 * *p00;
    *p01 -= k0 * *p01;
    *p10 -= k1 * *p00;
    *p11 -= k1 * *p01;
}

vector_t attitude(vector_t gyro, vector_t accel) {
    uint64_t curr_millis = timer_millis();
    double dt = (curr_millis - millis) * 0.001;
    millis = curr_millis;

    _attitude(gyro.x, accel.x, &angle.x, &bias.x, &p00.x, &p01.x, &p10.x, &p11.x, q_gyro.x, q_angle.x, r_angle.x, dt);
    _attitude(gyro.y, accel.y, &angle.y, &bias.y, &p00.y, &p01.y, &p10.y, &p11.y, q_gyro.y, q_angle.y, r_angle.y, dt);
    return angle;
}

uint8_t attitude_get_id() {
	return ATTITUDE_ALGORITHM_ID;
}

void attitude_reset() {
	angle.x = 0.0;
	angle.y = 0.0;
	angle.z = 0.0;
}

void attitude_get_params(double params[]) {
    params[0] = q_angle.x;
    params[1] = q_angle.y;
    params[2] = q_gyro.x;
    params[3] = q_gyro.y;
    params[4] = r_angle.x;
    params[5] = r_angle.y;
}

void attitude_set_params(double params[]) {
    q_angle.x = params[0];
    q_angle.y = params[1];
    q_gyro.x = params[2];
    q_gyro.y = params[3];
    r_angle.x = params[4];
    r_angle.y = params[5];
}

