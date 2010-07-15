#include "../motor.h"

#define MOTOR_FRONT_LEFT = MOTOR_A
#define MOTOR_FRONT_RIGHT = MOTOR_B
#define MOTOR_REAR_RIGHT = MOTOR_C
#define MOTOR_REAR_LEFT = MOTOR_D

double[] motor_cmd(double throttle, vector_t mv) {
    double[] result = {0,0,0,0};
    
    result[MOTOR_FRONT_LEFT] = _motor_limit(throttle + mv.y + mv.z);
    result[MOTOR_REAR_RIGHT] = _motor_limit(throttle - mv.y + mv.z);
    result[MOTOR_FRONT_RIGHT] = _motor_limit(throttle - mv.x - mv.z);
    result[MOTOR_REAR_LEFT] = _motor_limit(throttle + mv.x - mv.z);
    
    return result;
}

double _motor_limit(double value) {
    if (value > 1) return 1;
    if (value < 0) return 0;
    return value;
}