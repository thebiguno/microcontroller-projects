#define MOTOR_FRONT = MOTOR_A
#define MOTOR_RIGHT = MOTOR_B
#define MOTOR_REAR = MOTOR_C
#define MOTOR_LEFT = MOTOR_D

uint16_t[] motor_cmd(uint16_t throttle_cmd, uint16_t[] axis_cmd) {
    uint16_t[] motor_cmd = {0,0,0,0};
    motor_cmd[MOTOR_FRONT] = constrain(throttle_cmd + axis_cmd[PITCH] - axis_command[YAW], MOTOR_MINCOMMAND, MOTOR_MAXCOMMAND);
    motor_cmd[MOTOR_REAR] = constrain(throttle_cmd - axis_cmd[PITCH] - axis_command[YAW], MOTOR_MINCOMMAND, MOTOR_MAXCOMMAND);
    motor_cmd[MOTOR_RIGHT] = constrain(throttle_cmd + axis_cmd[ROLL] + axis_command[YAW], MOTOR_MINCOMMAND, MOTOR_MAXCOMMAND);
    motor_cmd[MOTOR_LEFT] = constrain(throttle_cmd - axis_cmd[ROLL] + axis_command[YAW], MOTOR_MINCOMMAND, MOTOR_MAXCOMMAND);
    return motor_cmd;
}