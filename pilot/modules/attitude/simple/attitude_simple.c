#include "../attitude.h"

#define ATTITUDE_ALGORITHM_ID 0x08

// TODO read timeConstant from EEPROM
// TODO try to understand what these filter terms are for
#define timeConstant 0
#define filterTerm0 timeConstant / (timeConstant + 0.010) //10ms = ESC update rate
#define filterTerm1 1 - filterTerm0

#define MAGIC 57.2957795 // what is this constant??

static vector_t pv;

void attitude_init(vector_t gyro, vector_t accel) {
    ;
}

void attitude_reset() {
    pv.x = 0;
    pv.y = 0;
    pv.z = 0;
}

vector_t attitude(vector_t gyro, vector_t accel, uint16_t dt) {
    vector_t result;
    result.x = (filterTerm0 * (pv.x + (gyro.x * dt)) + filterTerm1 * (accel.x)) * MAGIC;
    result.y = (filterTerm0 * (pv.y + (gyro.y * dt)) + filterTerm1 * (accel.y)) * MAGIC;
    return result;
}

uint8_t attitude_get_id() {
	return ATTITUDE_ALGORITHM_ID;
}

void attitude_get_params(double params[]) {

}

void attitude_set_params(double params[]) {

}
