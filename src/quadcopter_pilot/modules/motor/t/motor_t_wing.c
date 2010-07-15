#include "../motor.h"

#define MOTOR_FRONT 0
#define MOTOR_RIGHT 1
#define MOTOR_REAR 2
#define MOTOR_LEFT 3

double motor_cmd(double throttle, vector_t mv) {
    double[] result = {0,0,0,0};

    result[MOTOR_FRONT] = _motor_limit(throttle + mv.y - mv.z);
    result[MOTOR_REAR] = _motor_limit(throttle - mv.y - mv.z);
    result[MOTOR_RIGHT] = _motor_limit(throttle + mv.x + mv.z);
    result[MOTOR_LEFT] = _motor_limit(throttle - mv.x + mv.z);
    
    return result;
}

double _motor_limit(double value) {
    if (value > 1.0) return 1.0;
    if (value < 0.0) return 0.0;
    return value;
}

