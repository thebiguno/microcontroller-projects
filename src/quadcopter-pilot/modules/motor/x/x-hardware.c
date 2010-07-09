#define MOTOR_FRONT_LEFT = MOTOR_A
#define MOTOR_FRONT_RIGHT = MOTOR_B
#define MOTOR_REAR_RIGHT = MOTOR_C
#define MOTOR_REAR_LEFT = MOTOR_D

uint16_t[] motor_cmd(throttleCommandaxisCommand) {
    uint16_t[] motor_cmd = {0,0,0,0};
    motor_cmd[MOTOR_FRONT_LEFT] = limitRange(throttle_cmd + axis_cmd[PITCH] + axis_command[YAW]);
    motor_cmd[MOTOR_REAR_RIGHT] = limitRange(throttle_cmd - axis_cmd[PITCH] + axis_command[YAW]);
    motor_cmd[MOTOR_FRONT_RIGHT] = limitRange(throttle_cmd - axis_cmd[ROLL] - axis_command[YAW]);
    motor_cmd[MOTOR_REAR_LEFT] = limitRange(throttle_cmd + axis_cmd[ROLL] - axis_command[YAW]);
    return motor_cmd;
}
