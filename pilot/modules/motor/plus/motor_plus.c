#include "../motor.h"

#define MOTOR_LIMIT(x, t) (t > 0.01 ? (x > 1.0 ? 1.0 : (x < 0.0 ? 0.0 : x)) : 0.0)

static double motor_a_tuning = 1.0;
static double motor_b_tuning = 1.0;
static double motor_c_tuning = 1.0;
static double motor_d_tuning = 1.0;

void motor_percent(double throttle, vector_t mv, double result[]) {
    result[0] = MOTOR_LIMIT(motor_a_tuning * (throttle + mv.y + mv.z), throttle); // left, A
    result[1] = MOTOR_LIMIT(motor_b_tuning * (throttle + mv.x - mv.z), throttle); // front, B
    result[2] = MOTOR_LIMIT(motor_c_tuning * (throttle - mv.y + mv.z), throttle); // right, C
    result[3] = MOTOR_LIMIT(motor_d_tuning * (throttle - mv.x - mv.z), throttle); // back, D
}

void motor_init(){
	motor_read_tuning();
}

void motor_read_tuning(){
	uint8_t data[16];
	
	uint8_t length = persist_read(PERSIST_SECTION_MOTOR, data, 16);
	if (length == 16) {
		motor_a_tuning = convert_bytes_to_double(data, 0);
		motor_b_tuning = convert_bytes_to_double(data, 4);
		motor_c_tuning = convert_bytes_to_double(data, 8);
		motor_d_tuning = convert_bytes_to_double(data, 12);
	} else {
		motor_a_tuning = 1.0;
		motor_b_tuning = 1.0;
		motor_c_tuning = 1.0;
		motor_d_tuning = 1.0;
	}
}

void motor_write_tuning(){
	uint8_t data[16];
	
	convert_double_to_bytes(motor_a_tuning, data, 0);
	convert_double_to_bytes(motor_b_tuning, data, 4);
	convert_double_to_bytes(motor_c_tuning, data, 8);
	convert_double_to_bytes(motor_d_tuning, data, 12);
	
	persist_write(PERSIST_SECTION_MOTOR, data, 16);
}

void motor_send_tuning(){
	uint8_t data[16];
	
	convert_double_to_bytes(motor_a_tuning, data, 0);
	convert_double_to_bytes(motor_b_tuning, data, 4);
	convert_double_to_bytes(motor_c_tuning, data, 8);
	convert_double_to_bytes(motor_d_tuning, data, 12);

	protocol_send_message('m', data, 16);
}

void motor_receive_tuning(uint8_t *buf){
	motor_a_tuning = convert_bytes_to_double(buf, 0);
	motor_b_tuning = convert_bytes_to_double(buf, 4);
	motor_c_tuning = convert_bytes_to_double(buf, 8);
	motor_d_tuning = convert_bytes_to_double(buf, 12);
}