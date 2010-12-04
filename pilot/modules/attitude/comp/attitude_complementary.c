#include <math.h>

#include "../../../main.h"
#include "../../../lib/timer/timer.h"

#define DEG_PT5 0.00872664626
#define DEG_30 1.570796326794897
/*
 * A complementary filter derived from the following:
 * - http://www.rcgroups.com/forums/showpost.php?p=12082524&postcount=1286
 * - http://www.rcgroups.com/forums/showthread.php?p=11643539#post11643539
 */
static vector_t int_y1;    // output of the first integrator (rad/sec)
static vector_t filter;    // complementary filter output (and output of second integrator), (rad)
static vector_t k;         // tuning paramater -- bandwidth of filter
uint64_t millis;

void attitude_init(vector_t gyro, vector_t accel) {
    millis = timer_millis();
    
    // TODO read this from EEPROM
    k.x = 1;
    k.y = 1;
    k.z = 1;

    filter.x = accel.x;
    filter.y = accel.y;
    filter.z = accel.z;
    //account for gyro bias
    int_y1.x = -gyro.x;
    int_y1.y = -gyro.y;
    int_y1.z = -gyro.z;
}

void _attitude(double *gyro, double *_accel, double *_accel_z, double *_int_y1, double *_filter, double k, double dt) {
    double accel;
    if (*_accel_z < DEG_PT5 || *_filter > DEG_30 || *_filter < -DEG_30) {
        accel = *_accel;
    } else {
        accel = atan2(*_accel, *_accel_z);
    }

    double int_x1 = (accel - *_filter) * k *  k;                    // input to the first integrator (rad/sec/sec)
    *_int_y1 += int_x1 * dt;
    double int_x2 = *_int_y1 + (accel - *_filter) * 2 *  k + *gyro;  // input to the second integrator (rad/sec)
    *_filter = (int_x2 * dt) + *_filter;
}

vector_t attitude(vector_t gyro, vector_t accel) {
    uint64_t curr_millis = timer_millis();
    double dt = (curr_millis - millis) * 0.001;
    millis = curr_millis;

    _attitude(&gyro.x, &accel.x, &accel.z, &int_y1.x, &filter.x, k.x, dt);
    _attitude(&gyro.y, &accel.y, &accel.z, &int_y1.y, &filter.y, k.y, dt);
    
    return filter;
}

void attitude_reset() {
    filter.x = 0;
    filter.y = 0;
    filter.z = 0;
}

uint8_t attitude_get_id() {
    return 'C';
}

void attitude_get_params(double params[]) {

}

void attitude_set_params(double params[]) {

}
