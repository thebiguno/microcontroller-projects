#include "../motor.h"

#define MOTOR_LIMIT(x, t) (t > 0.01 ? (x > 1.0 ? 1.0 : (x < 0.0 ? 0.0 : x)) : 0.0)

void motor_percent(double throttle, vector_t mv, double result[]) {
    result[0] = MOTOR_LIMIT(throttle + mv.y + mv.z, throttle); // left, A
    result[1] = MOTOR_LIMIT(throttle + mv.x - mv.z, throttle); // front, B
    result[2] = MOTOR_LIMIT(throttle - mv.y + mv.z, throttle); // right, C
    result[3] = MOTOR_LIMIT(throttle - mv.x - mv.z, throttle); // back, D
}

void motor_init(){
}
