#include "../motor.h"

#define MOTOR_LIMIT(x) (x > 1.0 ? 1.0 : (x < 0.0 ? 0.0 : x))

void motor_percent(double throttle, vector_t mv, double result[]) {
    result[0] = MOTOR_LIMIT(throttle + mv.x + mv.y + mv.z); // front-left
    result[1] = MOTOR_LIMIT(throttle - mv.x + mv.y - mv.z); // front-right
    result[2] = MOTOR_LIMIT(throttle - mv.x - mv.y + mv.z); // rear-right
    result[3] = MOTOR_LIMIT(throttle + mv.x - mv.y - mv.z); // rear-left
}
