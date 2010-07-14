#include "modules/accel/accel.h"
#include "modules/attitude/attitude.h"
#include "modules/gyro/gyro.h"
#include "modules/motor/motor.h"
#include "modules/pid/pid.h"

void set_data(char *data){
	accel_set_data(data);
	attitude_set_data(data);
	gyro_set_data(data);
	motor_set_data(data);
	pid_set_data();
}

void send_debug_data(uint8_t *data){
	//TODO send 
}
