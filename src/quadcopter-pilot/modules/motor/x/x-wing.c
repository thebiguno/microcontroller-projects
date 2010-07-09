#define MOTOR_FRONT_LEFT = MOTOR_A
#define MOTOR_FRONT_RIGHT = MOTOR_B
#define MOTOR_REAR_RIGHT = MOTOR_C
#define MOTOR_REAR_LEFT = MOTOR_D

uint16_t[] motor_cmd(uint16_t throttle_cmd, uint16_t[] axis_cmd) {
    uint16_t[] motor_cmd = {0,0,0,0};
    
    motor_cmd[MOTOR_FRONT_LEFT] = chkadd(throttle_cmd, chkadd(axis_cmd[PITCH], axis_command[YAW]));
    motor_cmd[MOTOR_REAR_RIGHT] = chksub(throttle_cmd, chkadd(axis_cmd[PITCH], axis_command[YAW]));
    motor_cmd[MOTOR_FRONT_RIGHT] = chksub(throttle_cmd, chksub(axis_cmd[ROLL], axis_command[YAW]));
    motor_cmd[MOTOR_REAR_LEFT] = chkadd(throttle_cmd, chksub(axis_cmd[ROLL], axis_command[YAW]));
    return motor_cmd;
}

uint16_t chkadd(uint16_t a, uint16_t b) {
    if (0xFFFF - b < a) return 0xFFFF;
    return a + b;
}

uint16_t chksub(uint16_t a, uint16_t b) {
    if (b > a) return 0x0000;
    return a - b;
}
