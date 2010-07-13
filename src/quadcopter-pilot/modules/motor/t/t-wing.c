#define MOTOR_FRONT = MOTOR_A
#define MOTOR_RIGHT = MOTOR_B
#define MOTOR_REAR = MOTOR_C
#define MOTOR_LEFT = MOTOR_D

float motor_cmd(float[] throttle_cmd, float[] axis_cmd) {
    float[] motor_cmd = {0,0,0,0};
    motor_cmd[MOTOR_FRONT] = throttle_cmd + axis_cmd[PITCH] - axis_command[YAW];
    motor_cmd[MOTOR_REAR] = throttle_cmd - axis_cmd[PITCH] - axis_command[YAW];
    motor_cmd[MOTOR_RIGHT] = throttle_cmd + axis_cmd[ROLL] + axis_command[YAW];
    motor_cmd[MOTOR_LEFT] = throttle_cmd - axis_cmd[ROLL] + axis_command[YAW];
    return motor_cmd;
}