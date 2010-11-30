#include "../attitude.h"
#include "../../lib/timer/timer.h"

#define ATTITUDE_ALGORITHM_ID 0x08

// TODO read kt from EEPROM
// TODO try to understand what these filter terms are for
#define kt 0
#define k0 kt / (kt + 0.010) //10ms = ESC update rate
#define k1 1 - k0

static vector_t pv;
uint64_t millis = 0;

void attitude_init(vector_t gyro, vector_t accel) {
    millis = timer_millis();
}

void attitude_reset() {
    millis = 0;
    pv.x = 0;
    pv.y = 0;
    pv.z = 0;
}

vector_t attitude(vector_t gyro, vector_t accel) {
    uint64_t curr_millis = timer_millis();
    double dt = (curr_millis - millis) * 0.001;
    millis = curr_millis;
    pv.x = (k0 * (pv.x + (gyro.x * dt)) + k1 * (accel.x));
    pv.y = (k0 * (pv.y + (gyro.y * dt)) + k1 * (accel.y));
    pv.z = (k0 * (pv.z + (gyro.z * dt)) + k1 * (accel.z));
    pv;
}

uint8_t attitude_get_id() {
	return ATTITUDE_ALGORITHM_ID;
}

void attitude_get_params(double params[]) {

}

void attitude_set_params(double params[]) {

}
