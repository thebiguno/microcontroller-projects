#define MOTOR_A = 0
#define MOTOR_B = 1
#define MOTOR_C = 2
#define MOTOR_D = 3

/*
 * given a throttle percentage (0 .. 1) and vector of manipulated variables in rad, calculate the motor power percentage (0..1)
 */
double[] motor_cmd(double throttle, vector_t mv);
