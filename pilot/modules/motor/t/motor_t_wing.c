#include "../motor.h"

#define MOTOR_LIMIT(x) (x > 1.0 ? 1.0 : (x < 0.0 ? 0.0 : x))

void motor_percent(double throttle, vector_t mv, double result[]) {
    result[0] = MOTOR_LIMIT(throttle + mv.y + mv.z); // left
    result[1] = MOTOR_LIMIT(throttle - mv.x - mv.z); // front
    result[2] = MOTOR_LIMIT(throttle - mv.y + mv.z); // right
    result[3] = MOTOR_LIMIT(throttle + mv.x - mv.z); // rear
}
