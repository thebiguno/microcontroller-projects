#include "../motor.h"

#define MOTOR_FRONT = MOTOR_A
#define MOTOR_RIGHT = MOTOR_B
#define MOTOR_REAR = MOTOR_C
#define MOTOR_LEFT = MOTOR_D

double motor_cmd(float throttle, vector_t mv) {
    double[] result = {0,0,0,0};
    
    result[MOTOR_FRONT] = _motor_limit(throttle + mv.y - mv.z);
    result[MOTOR_REAR] = _motor_limit(throttle - mv.y - mv.z);
    result[MOTOR_RIGHT] = _motor_limit(throttle + mv.x + mv.z);
    result[MOTOR_LEFT] = _motor_limit(throttle - mv.x + mv.z);
    
    return result;
}

double _motor_limit(double value) {
    if (value > 1) return 1;
    if (value < 0) return 0;
    return value;
}

