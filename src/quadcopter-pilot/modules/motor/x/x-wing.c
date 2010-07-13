#define MOTOR_FRONT_LEFT = MOTOR_A
#define MOTOR_FRONT_RIGHT = MOTOR_B
#define MOTOR_REAR_RIGHT = MOTOR_C
#define MOTOR_REAR_LEFT = MOTOR_D

float[] motor_cmd(float throttle_cmd, float[] axis_cmd) {
    float[] motor_cmd = {0,0,0,0};
    
    motor_cmd[MOTOR_FRONT_LEFT] = throttle_cmd + axis_cmd[PITCH] + axis_command[YAW];
    motor_cmd[MOTOR_REAR_RIGHT] = throttle_cmd - axis_cmd[PITCH] + axis_command[YAW];
    motor_cmd[MOTOR_FRONT_RIGHT] = throttle_cmd - axis_cmd[ROLL] - axis_command[YAW];
    motor_cmd[MOTOR_REAR_LEFT] = throttle_cmd + axis_cmd[ROLL] - axis_command[YAW];
    return motor_cmd;
}