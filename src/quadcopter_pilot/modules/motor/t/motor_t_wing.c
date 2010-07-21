#include "../motor.h"

#define MOTOR_FRONT 0
#define MOTOR_RIGHT 1
#define MOTOR_REAR 2
#define MOTOR_LEFT 3

#define MOTOR_LIMIT(x) (x > 1.0 ? 1.0 : (x < 0.0 ? 0.0 : x))

void motor_cmd(double *result, double throttle, vector_t mv) {
    result[MOTOR_FRONT] = MOTOR_LIMIT(throttle + mv.y - mv.z);
    result[MOTOR_REAR] = MOTOR_LIMIT(throttle - mv.y - mv.z);
    result[MOTOR_RIGHT] = MOTOR_LIMIT(throttle + mv.x + mv.z);
    result[MOTOR_LEFT] = MOTOR_LIMIT(throttle - mv.x + mv.z);
}
