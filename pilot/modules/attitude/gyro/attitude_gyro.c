#include <math.h>

#include "../../../main.h"
#include "../../../lib/timer/timer.h"

static vector_t angle;

void attitude_read_tuning() {

}

void attitude_write_tuning() {

}

void attitude_init(vector_t gyro, vector_t accel) {

}

vector_t attitude(vector_t gyro, vector_t accel) {
// http://diydrones.com/forum/topics/order-of-gyro-integration-in

	vector_t dot;
	dot.y = gyro.y + sin(angle.y) * tan(angle.x * gyro.x) + cos(angle.y) * tan(angle.x * gyro.z);
	dot.x = cos(angle.y * gyro.x) + sin(angle.y * gyro.z);
	dot.z = (sin(angle.y) / cos(angle.x)) * gyro.x + (cos(angle.y) / cos(angle.x)) * gyro.z;

	angle.y = dot.y;
	angle.x = dot.x;
	angle.z = dot.z;
    
    return angle;
}

void attitude_reset() {
    angle.x = 0;
    angle.y = 0;
    angle.z = 0;
}

uint8_t attitude_get_id() {
    return 'C';
}

void attitude_send_tuning() {

}

void attitude_receive_tuning(uint8_t *buf) {

}
