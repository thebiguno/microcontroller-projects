#include <math.h>
#include <util/delay.h>

#include "../../../main.h"

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

void _attitude_read_tuning() {
	uint8_t data[24];
	
	uint8_t length = persist_read(PERSIST_SECTION_ATTITUDE, data, 8);
	if (length == 8) {
		k.x = convert_bytes_to_double(data, 0);
		k.y = convert_bytes_to_double(data, 4);
	} else {
		status_error(STATUS_ERR_ATTITUDE_CHECKSUM);
		_delay_ms(1000);
		status_error(0x00);
		
		k.x = 1;
		k.y = 1;
	}
}

void _attitude_write_tuning() {
	uint8_t data[8];
	
	convert_double_to_bytes(k.x, data, 0);
	convert_double_to_bytes(k.y, data, 4);
	
	persist_write(PERSIST_SECTION_ATTITUDE, data, 8);
}

void attitude_init(vector_t gyro, vector_t accel) {
	_attitude_read_tuning();

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
	if (*_accel_z < 0.5 || *_filter > DEG_30 || *_filter < -DEG_30) {
		// if the craft is falling very fast, or tilted more than 30 degrees
		// then ignore the accelerometer (accel - *_filter will be zero later on)
		accel = *_filter;
	} else {
		accel = atan2(*_accel, *_accel_z);
	}

	double int_x1 = (accel - *_filter) * k * k;							// input to the first integrator (rad/s/s) -- rate of change
	*_int_y1 += int_x1 * dt;											// (rad/s)
	double int_x2 = *_int_y1 + (accel - *_filter) * 2 * k + *gyro;		// input to the second integrator (rad/s)
	*_filter += int_x2 * dt;											// (rad)
}

vector_t attitude(vector_t gyro, vector_t accel, double dt) {
	_attitude(&gyro.x, &accel.x, &accel.z, &int_y1.x, &filter.x, k.x, dt);
	_attitude(&gyro.y, &accel.y, &accel.z, &int_y1.y, &filter.y, k.y, dt);
	return filter;
}

void attitude_reset() {
	filter.x = 0;
	filter.y = 0;
}

uint8_t attitude_get_id() {
	return 'C';
}

void attitude_send_tuning() {
	uint8_t length = 8;
	uint8_t buf[length];
	
	convert_double_to_bytes(k.x, buf, 0);
	convert_double_to_bytes(k.y, buf, 4);

	protocol_send_message('c', buf, length);
}

void attitude_receive_tuning(uint8_t *buf) {
	k.x = convert_bytes_to_double(buf, 0);
	k.y = convert_bytes_to_double(buf, 4);
	
	_attitude_write_tuning();
}
